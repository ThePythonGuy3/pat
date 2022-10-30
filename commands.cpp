#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>

#include "crypto/md5.h"
#include "zlib/zlib.h"
#include "zlib/highZlib.h"
#include "commands.h"
#include "filedata.h"

#ifdef _WIN32
#include <Windows.h>
#endif

// Check if the command syntax is correct.
bool checkCommand(int argc, char *argv[], std::filesystem::path patPath, std::string commandStr, int argumentsRequired, bool pushError)
{
    const char *command = commandStr.c_str();

    if (strcmp(argv[1], command) == 0)
    {

        if (commandStr != "init" && commandStr != "help" && !std::filesystem::exists(patPath))
        {
            std::cout << "The current directory is not PAT-enabled. Activate PAT by using the 'pat init' command.";
            return false;
        }

        bool result = argumentsRequired == -1 || argc == (argumentsRequired + 2);

        if (pushError && !result)
        {
            std::cout << "Wrong syntax for command '" << commandStr << "'. The command takes " << argumentsRequired << (argumentsRequired == 1 ? " argument" : " arguments") << " but " << (argc - 2) << ((argc - 2) == 1 ? " was" : " were") << " given. See 'pat help'." << std::endl;
        }

        return result;
    }

    return false;
}

bool checkCommand(int argc, char *argv[], std::filesystem::path patPath, std::string commandStr, int argumentsRequired)
{
    return checkCommand(argc, argv, patPath, commandStr, argumentsRequired, true);
}

// init command code. Used both in init and reset.
bool initStart(std::filesystem::path patPath, char *patPathArray)
{
    bool successOnCreation = std::filesystem::create_directory(patPath);
    if (!std::filesystem::create_directory(patPath / "objects")) // Object storage.
        successOnCreation = false;
    if (!std::filesystem::create_directory(patPath / "cmit")) // Tracked but non-commited files.
        successOnCreation = false;
    if (!std::filesystem::create_directory(patPath / "meta")) // Metadata, makes tracking faster.
        successOnCreation = false;

#ifdef _WIN32
    // Only hide the folder in Windows, UNIX based OS' hide folders whose name starts with a '.' automatically.
    if (!SetFileAttributes(patPathArray, FILE_ATTRIBUTE_HIDDEN))
        successOnCreation = false;
#endif

    return successOnCreation;
}

//-----

void init(std::filesystem::path patPath, char *patPathArray)
{
    if (!std::filesystem::exists(patPath))
    {
        bool successOnCreation = initStart(patPath, patPathArray);

        if (successOnCreation)
        {
            std::cout << "PAT initialized." << std::endl;
        }
        else
        {
            std::cout << "PAT failed to initialize here." << std::endl;
        }
    }
    else
    {
        std::cout << "PAT already enabled in this directory." << std::endl;
    }
}

void track(std::filesystem::path workingPath, std::filesystem::path patPath, std::string patTreeHash)
{
    std::vector<std::string> treeHashes;                         // All hashes for tree type objects
    std::vector<std::string> mainObjects;                        // All hashes for objects in the main directory
    std::map<std::string, std::vector<std::string>> treeHashMap; // A hash(tree)->hash(object) map. Stores temporary data.

    using iterator = std::filesystem::recursive_directory_iterator;

    std::filesystem::path mfileP = ((patPath / "meta") / "modFile");
    bool modFileExists = std::filesystem::exists(mfileP);
    std::string preModFile = "";

    if (modFileExists)
    {
        std::ifstream mfileS(mfileP.string(), std::ios::binary);
        std::stringstream preModFileR;/*((std::istreambuf_iterator<char>(mfileS)),
                                (std::istreambuf_iterator<char>()));*/
        preModFileR << mfileS.rdbuf();
        preModFile = preModFileR.str();
        mfileS.close();

        std::string decompModFile = highZlib::decompress_string(preModFile);

        std::stringstream dStream(decompModFile);
        std::string line;
        while (std::getline(dStream, line, '\n'))
        {
            size_t fSemicolon = line.find(';');
            std::string linePath = line.substr(0, fSemicolon);
            std::string rest = line.substr(fSemicolon + 1);

            size_t sSemicolon = rest.find(';');
            long long uTime = std::stoll(rest.substr(0, sSemicolon));
            long long chLen = std::stoll(rest.substr(sSemicolon + 1, rest.size() - 1));

            long long* currentData = readMetadata(linePath);

            if(uTime != currentData[0] || chLen != currentData[1])
            {
                std::cout << linePath + " has been updated.";
            }
        }
    }

    std::string modFile = "";
    for (const auto &value : iterator(workingPath))
    {
        std::filesystem::path path = value.path();
        std::string pathString = path.string();

        if (pathString.find(patPath.string()) == std::string::npos)
        {
            int dataType = 0; // 0 = file, 1 = directory
            if (std::filesystem::is_directory(path))
            {
                dataType = 1;
            }

            std::string fcontent, hash, tag, name;
            std::string parentHash = md5(path.parent_path());

            if (dataType == 0)
            {
                std::ifstream ifs(pathString, std::ios::binary);
                std::string content((std::istreambuf_iterator<char>(ifs)),
                                    (std::istreambuf_iterator<char>()));
                fcontent = "blob " + std::to_string(content.length()) + "\n" + path.string() + "\n" + content;

                hash = md5(fcontent);
                ifs.close();
            }
            else
            {
                hash = md5(path);

                treeHashes.push_back(hash);
            }

            modFile += pathString + ";" + readMetadataStr(path) + "\n";

            if (parentHash != patTreeHash)
            {
                if (!treeHashMap.count(parentHash))
                {
                    std::vector<std::string> children;
                    treeHashMap.insert(std::pair<std::string, std::vector<std::string>>(parentHash, children));
                }

                treeHashMap[parentHash].push_back((dataType ? "tree " : "blob ") + hash + "\n");
            }
            else
            {
                mainObjects.push_back(hash);
            }

            if (dataType == 0)
            {
                tag = hash.substr(0, 2);
                name = hash.substr(2, hash.length());

                std::filesystem::create_directories((patPath / "objects") / tag);

                std::string compressed = highZlib::compress_string(fcontent);

                std::ofstream file(((patPath / "objects") / tag) / name, std::ios::binary);
                file << compressed;

                file.close();
            }
        }
    }

    modFile.pop_back();

    std::filesystem::create_directories(patPath / "meta");

    std::ofstream metaFile((patPath / "meta") / "modFile", std::ios::binary);
    metaFile << highZlib::compress_string(modFile);

    metaFile.close();

    for (int i = 0; i < treeHashes.size(); i++)
    {
        std::vector<std::string> hashes = treeHashMap[treeHashes[i]];
        std::string value = "tree " + std::to_string(hashes.size()) + "\n";

        for (int j = 0; j < hashes.size(); j++)
        {
            value += hashes[j];
        }

        std::string tag, name, hash = treeHashes[i];

        tag = hash.substr(0, 2);
        name = hash.substr(2, hash.length());

        std::filesystem::create_directories((patPath / "objects") / tag);

        std::string compressed = highZlib::compress_string(value);

        std::ofstream file(((patPath / "objects") / tag) / name, std::ios::binary);
        file << compressed;

        file.close();
    }
}

void peek(char *argv[], std::filesystem::path patPath)
{
    std::string arg = argv[2], tag, name;

    tag = arg.substr(0, 2);
    name = arg.substr(2, arg.length());

    std::filesystem::path readPath = ((patPath / "objects") / tag) / name;
    if (std::filesystem::exists(readPath))
    {
        std::ifstream ifs(readPath, std::ios::binary);
        std::string content((std::istreambuf_iterator<char>(ifs)),
                            (std::istreambuf_iterator<char>()));

        std::cout << highZlib::decompress_string(content) << std::endl;
        ifs.close();
    }
    else
    {
        std::cout << "Object '" << argv[2] << "' not found.";
    }
}

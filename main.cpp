// Project Administration Tool (PAT) by @ThePythonGuy3
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "./crypto/md5.cpp"
#include "./zlib/zlib.h"
#include "./zlib/highZlib.cpp"

#ifdef _WIN32
#include <Windows.h>
#endif

const int cmdLen = 7;
std::string cmdNames[cmdLen] = {"init", "track", "register", "push", "copy", "peek", "help"};
std::string helpQuotes[cmdLen] = {"Enables PAT for the current directory. 'pat init'", "Tracks all changes done to the working directory. 'pat track'", "Registers the current changes under a name. 'pat register {name}'", "Pushes the changes made into the remote server's project. 'pat push'", "Copies a certain PAT project from the remote server. 'pat copy {remote server project name}'", "Reads the data of an object. 'pat peek {hash}'"};

// Check if the command syntax is correct.
bool checkCommand(int argc, char *argv[], std::filesystem::path patPath, std::string commandStr, int argumentsRequired, bool pushError = true)
{
	if (commandStr != "init" && !std::filesystem::exists(patPath))
	{
		std::cout << "The current directory is not PAT-enabled. Activate PAT by using the 'pat init' command.";
		return false;
	}

	const char *command = commandStr.c_str();

	if (strcmp(argv[1], command) == 0)
	{
		bool result = argumentsRequired == -1 || argc == (argumentsRequired + 2);

		if (pushError && !result)
		{
			std::cout << "Wrong syntax for command '" << commandStr << "'. The command takes " << argumentsRequired << (argumentsRequired == 1 ? " argument" : " arguments") << " but " << (argc - 2) << ((argc - 2) == 1 ? " was" : " were") << " given. See 'pat help'." << std::endl;
		}

		return result;
	}

	return false;
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

int main(int argc, char *argv[])
{
	// The working directory path.
	std::filesystem::path workingPath = std::filesystem::canonical(".");

	// The .pat/ directory path inside of the working directory.
	std::filesystem::path patPath = workingPath / ".pat";
	std::string patTreeHash = md5(workingPath);

	int n = patPath.string().length();

	// The .pat/ directory path inside the working directory in string form.
	char *patPathArray = new char[n + 1];
	strcpy(patPathArray, patPath.string().c_str());

	// One single argument. Pops out help.
	if (argc <= 1)
	{
		std::cout << "Welcome to PAT, insert help here blah blah..." << std::endl;
	}
	else
	{
		bool valid = false;
		for (int i = 0; i < cmdLen; i++)
		{
			if (argv[1] == cmdNames[i])
			{
				valid = true;
				break;
			}
		}

		if (valid)
		{
			// 'init' command. Enables PAT for the current directory. 'pat init'
			if (checkCommand(argc, argv, patPath, "init", 0))
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

			// 'track' command. Tracks all changes done to the working directory. 'pat track'
			if (checkCommand(argc, argv, patPath, "track", 0))
			{
				std::vector<std::string> treeHashes;						 // All hashes for tree type objects
				std::vector<std::string> mainObjects;						 // All hashes for objects in the main directory
				std::map<std::string, std::vector<std::string>> treeHashMap; // A hash(tree)->hash(object) map. Stores temporary data.

				using iterator = std::filesystem::recursive_directory_iterator;

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
							std::ifstream ifs(pathString);
							std::string content((std::istreambuf_iterator<char>(ifs)),
												(std::istreambuf_iterator<char>()));
							fcontent = "blob " + std::to_string(content.length()) + "\n" + content;

							hash = md5(fcontent);
						}
						else
						{
							hash = md5(path);

							treeHashes.push_back(hash);
						}

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

							std::ofstream file(((patPath / "objects") / tag) / name);
							file << compressed;

							file.close();
						}
					}
				}

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

					std::ofstream file(((patPath / "objects") / tag) / name);
					file << compressed;

					file.close();
				}
			}

			// 'register' command. Registers the current changes under a name. 'pat register {name}'
			if (checkCommand(argc, argv, patPath, "register", 1))
			{
				std::cout << argc;
			}

			// 'push' command. Pushes the changes made into the remote server's project. 'pat push'
			if (checkCommand(argc, argv, patPath, "push", 0))
			{
				std::cout << argc;
			}

			// 'copy' command. Copies a certain PAT project from the remote server. 'pat copy {remote server project name}'
			if (checkCommand(argc, argv, patPath, "copy", 1))
			{
				std::cout << argc;
			}

			// 'peek' command. Reads the data of an object. 'pat peek {hash}'
			if (checkCommand(argc, argv, patPath, "peek", 1))
			{
				std::string arg = argv[2], tag, name;

				tag = arg.substr(0, 2);
				name = arg.substr(2, arg.length());

				std::filesystem::path readPath = ((patPath / "objects") / tag) / name;
				if (std::filesystem::exists(readPath))
				{
					std::ifstream ifs(readPath);
					std::string content((std::istreambuf_iterator<char>(ifs)),
										(std::istreambuf_iterator<char>()));

					std::cout << highZlib::decompress_string(content) << std::endl;
				}
				else
				{
					std::cout << "Object '" << argv[2] << "' not found.";
				}
			}

			// 'help' command.
			if (checkCommand(argc, argv, patPath, "help", 0, false))
			{
				std::cout << "general" << std::endl;
			}
			else if (checkCommand(argc, argv, patPath, "help", 1, false))
			{
				std::cout << "specific" << std::endl;
			}
			else if (strcmp(argv[1], "help") == 0)
			{
				std::cout << "Wrong syntax for command 'help'. The command takes at most 1 argument but " << (argc - 2) << ((argc - 2) == 1 ? " was" : " were") << " given. See 'pat help'." << std::endl;
			}
		}
		else
		{
			std::cout << "'" << argv[1] << "' is not a PAT command. See 'pat help'." << std::endl;
		}
	}

	std::cout << std::endl;
}
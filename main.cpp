// Project Administration Tool (PAT) by @ThePythonGuy3
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "./crypto/md5.cpp"
#include "./zlib/zlib.h"

#ifdef _WIN32
#include <Windows.h>
#endif

const int cmdLen = 6;
std::string cmdNames[cmdLen] = {"init", "track", "register", "push", "copy", "help"};
std::string helpQuotes[cmdLen] = {"Enables PAT for the current directory. 'pat init'", "Tracks all changes done to the working directory. 'pat track'", "Registers the current changes under a name. 'pat register {name}'", "Pushes the changes made into the remote server's project. 'pat push'", "Copies a certain PAT project from the remote server. 'pat copy {remote server project name}'"};

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

bool initStart(std::filesystem::path patPath, char *patPathArray)
{
	bool successOnCreation = std::filesystem::create_directory(patPath);
	if (!std::filesystem::create_directory(patPath / "objects"))
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
				using iterator = std::filesystem::recursive_directory_iterator;

				for (const auto &value : iterator(workingPath))
				{
					if (value.path().string().find(patPath.string()) == std::string::npos)
					{
						std::string hash = md5(value.path());

						int dataType = 0; // 0 = file, 1 = directory
						if (std::filesystem::is_directory(value.path()))
						{
							dataType = 1;
						}
					}
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
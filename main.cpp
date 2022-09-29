// Project Administration Tool (PAT) by @ThePythonGuy3
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "./crypto/md5.cpp"

#ifdef _WIN32
#include <Windows.h>
#endif

std::string cmdNames[6] = {"init", "track", "register", "push", "copy", "help"};
std::string helpQuotes[6] = {"Enables PAT for the current directory. 'pat init'", "Tracks all changes done to the working directory. 'pat track'", "Registers the current changes under a name. 'pat register {name}'", "Pushes the changes made into the remote server's project. 'pat push'", "Copies a certain PAT project from the remote server. 'pat copy {remote server project name}'"};

// Check if the command syntax is correct.
bool checkCommand(int argc, char *argv[], std::filesystem::path patPath, std::string commandStr, int argumentsRequired)
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

		if (!result)
		{
			std::cout << "Wrong syntax for command '" << commandStr << "'. The command takes " << argumentsRequired << (argumentsRequired == 1 ? " argument" : " arguments") << " but " << (argc - 2) << ((argc - 2) == 1 ? " was" : " were") << " given. See 'pat help'." << std::endl;
		}

		return result;
	}

	return false;
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
		for (int i = 0; i < cmdNames->length(); i++)
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
					bool successOnCreation = std::filesystem::create_directory(patPath);
					if (!std::filesystem::create_directory(patPath / "objects"))
						successOnCreation = false;

#ifdef _WIN32
					// Only hide the folder in Windows, UNIX based OS' hide folders whose name starts with a '.' automatically.
					if (!SetFileAttributes(patPathArray, FILE_ATTRIBUTE_HIDDEN))
						successOnCreation = false;
#endif

					std::ofstream ofs(patPath / "history");
					ofs.close();

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

			// 'help' command. General. No arguments.
			if (checkCommand(argc, argv, patPath, "help", 0))
			{
				std::cout << "general" << std::endl;
			}

			// 'help' command. Gives help about a certain command. 'pat help {command name}'
			if (checkCommand(argc, argv, patPath, "help", 1))
			{
				std::cout << "specific" << std::endl;
			}
		}
		else
		{
			std::cout << "'" << argv[1] << "' is not a PAT command. See 'pat help'." << std::endl;
		}
	}

	std::cout << std::endl;
}
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
#include "./commands.cpp"
#include "./filedata.cpp"

#ifdef _WIN32
#include <Windows.h>
#endif

const int cmdLen = 7;
std::string cmdNames[cmdLen] = {
	"init",
	"track",
	"register",
	"push",
	"copy",
	"peek",
	"help"};

std::string helpQuotes[cmdLen] = {
	"Enables PAT for the current directory. 'pat init'",
	"Tracks all changes done to the working directory. 'pat track'",
	"Registers the current changes under a name. 'pat register {name}'",
	"Pushes the changes made into the remote server's project. 'pat push'",
	"Copies a certain PAT project from the remote server. 'pat copy {remote server project name}'",
	"Reads the data of an object. 'pat peek {hash}'",
	"Displays help. 'pat help' (general) / 'pat help {command}' (specific)."};

void specificHelp(std::string command)
{
	bool valid = false;
	int pos = 0;
	for (int i = 0; i < cmdLen; i++)
	{
		if (command == cmdNames[i])
		{
			pos = i;
			valid = true;
			break;
		}
	}

	if (valid)
	{
		std::cout << "'" << command << "': " << helpQuotes[pos] << std::endl;
	}
	else
	{
		std::cout << "Command '" << command << "' doesn't exist." << std::endl;
	}
}

void generalHelp()
{
	std::cout << "Commands: " << std::endl;

	for (int i = 0; i < cmdLen; i++)
	{
		specificHelp(cmdNames[i]);
	}
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
		std::cout << "Welcome to PAT." << std::endl;
		generalHelp();
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
				init(patPath, patPathArray);
			}

			// 'track' command. Tracks all changes done to the working directory. 'pat track'
			if (checkCommand(argc, argv, patPath, "track", 0))
			{
				track(workingPath, patPath, patTreeHash);
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
				peek(argv, patPath);
			}

			// 'help' command.
			if (checkCommand(argc, argv, patPath, "help", 0, false))
			{
				generalHelp();
			}
			else if (checkCommand(argc, argv, patPath, "help", 1, false))
			{
				specificHelp(argv[2]);
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
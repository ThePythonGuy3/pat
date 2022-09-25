#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#endif

bool checkCommand(int argc, char *argv[], std::string cmd, int argumentsRequired)
{
	const char *command = cmd.c_str();

	if (strcmp(argv[1], command) == 0)
	{
		return argumentsRequired == -1 || argc == (argumentsRequired + 2);
	}

	return false;
}

int main(int argc, char *argv[])
{
	std::filesystem::path working_dir = std::filesystem::canonical(".");
	std::filesystem::path composedPath = working_dir += "\\.pnvc";
	int n = composedPath.string().length();

	char *composedPathArray = new char[n + 1];

	strcpy(composedPathArray, composedPath.string().c_str());

	if (argc <= 1)
	{
		std::cout << "Welcome to PNVC, insert help here blah blah..." << std::endl;
	}
	else
	{
		// Init command
		if (checkCommand(argc, argv, "init", 0))
		{

			if (!std::filesystem::exists(composedPath))
			{
				std::error_code ec;
				bool successOnCreation = std::filesystem::create_directory(composedPath, ec);

#ifdef _WIN32

				bool successOnHiding = SetFileAttributes(composedPathArray, FILE_ATTRIBUTE_HIDDEN);

				if (!successOnHiding)
					successOnCreation = false;
#endif

				// TODO main files creation.

				if (successOnCreation)
				{
					std::cout << "PNVC initialized." << std::endl;
				}
				else
				{
					std::cout << "ERROR: " << ec.message() << std::endl
							  << "PNVC failed to initialize here." << std::endl;
				}
			}
		}

		// Add command
		if (strcmp(argv[1], "add") == 0)
		{
			if (argc >= 2)
			{
			}
		}

		// Commit command
		if (strcmp(argv[1], "commit") == 0)
		{
			if (argc >= 2)
			{
			}
		}

		std::cout << std::endl;
	}
}
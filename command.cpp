#include <iostream>
#include <filesystem>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

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
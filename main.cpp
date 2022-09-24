#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <string>
#include <libloaderapi.h>
#include <Windows.h>
#include <sys/stat.h>

bool doesPathExist(const std::string &s)
{
	struct stat buffer;
	return (stat(s.c_str(), &buffer) == 0);
}

int main(int argc, char *argv[])
{
	char runPath[256];

	GetModuleFileNameA(NULL, runPath, sizeof(runPath));

	std::string runPathString = std::string(runPath);
	int barPos = runPathString.find_last_of('\\');
	std::string composedPath = runPathString.substr(0, barPos) + "\\.pnvc";

	int n = composedPath.length();
	char composedPathArray[n + 1];

	strcpy(composedPathArray, composedPath.c_str());

	if (!doesPathExist(composedPath))
	{
		CreateDirectory(composedPathArray, NULL);
		SetFileAttributes(composedPathArray, FILE_ATTRIBUTE_HIDDEN);
	}

	if (argc <= 1)
	{
		std::cout << "Welcome to PNVC, insert help here blah blah...";
	}
	else
	{
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
	}
}
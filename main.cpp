#include <stdio.h>
#include <cstring>
#include <string>
#include <filesystem>

#ifdef _WIN32
bool windows = true;
#include <libloaderapi.h>
#endif

#ifdef __unix__
bool windows = false;
#endif

// test
int main(int argc, char *argv[])
{
	if (windows)
	{
		char pBuf[256];

		GetModuleFileNameA(NULL, pBuf, sizeof(pBuf));

		printf(pBuf);
	}
	else
	{
		std::filesystem::canonical("/proc/self/exe/");
	}

	if (argc <= 1)
	{
		printf("Welcome to PNVC, insert help here blah blah...");
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
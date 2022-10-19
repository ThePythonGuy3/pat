#ifndef COMMANDS
#define COMMANDS

bool checkCommand(int argc, char *argv[], std::filesystem::path patPath, std::string commandStr, int argumentsRequired);
bool checkCommand(int argc, char *argv[], std::filesystem::path patPath, std::string commandStr, int argumentsRequired, bool pushError);
bool initStart(std::filesystem::path patPath, char *patPathArray);
void init(std::filesystem::path patPath, char *patPathArray);
void track(std::filesystem::path workingPath, std::filesystem::path patPath, std::string patTreeHash);
void peek(char *argv[], std::filesystem::path patPath);

#endif

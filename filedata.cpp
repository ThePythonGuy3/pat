#include <filesystem>
#include <string>
#include <iostream>

typedef long long llint;

template <typename TP>
std::time_t to_time_t(TP tp)
{
	using namespace std::chrono;
	auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
	return system_clock::to_time_t(sctp);
}

std::string readMetadata(std::filesystem::path path)
{
	if (std::filesystem::exists(path))
	{
		std::filesystem::file_time_type mtime = std::filesystem::last_write_time(path);

		llint unixMtime = to_time_t<std::filesystem::file_time_type>(mtime); // Unix modification time
		llint size = -1;													 // File size, -1 for non-regular files/directories.

		if (std::filesystem::is_regular_file(path))
		{
			size = std::filesystem::file_size(path);
		}

		std::cout << unixMtime + " " << size << std::endl;

		return std::to_string(size) + ";";
	}

	return "yo moma";
}
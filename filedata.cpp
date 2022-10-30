#include <filesystem>
#include <string>
#include <iostream>
#include "filedata.h"

template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}

long long* readMetadata(std::filesystem::path path)
{
    static long long op[] = {0, 0};

    if (std::filesystem::exists(path))
    {
        std::filesystem::file_time_type mtime = std::filesystem::last_write_time(path);

        long long unixMtime = to_time_t<std::filesystem::file_time_type>(mtime); // Unix modification time
        long long size = -1;                                                     // File size, -1 for non-regular files/directories.

        if (std::filesystem::is_regular_file(path))
        {
            size = std::filesystem::file_size(path);
        }

        op[0] = unixMtime;
        op[1] = size;
    }

    return op;
}

std::string readMetadataStr(std::filesystem::path path)
{
    long long* op = readMetadata(path);

    return std::to_string(op[0]) + ";" + std::to_string(op[1]) + ";";
}

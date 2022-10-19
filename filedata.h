#ifndef FILEDATA
#define FILEDATA

template <typename TP>
std::time_t to_time_t(TP tp);

std::string readMetadata(std::filesystem::path path);

#endif

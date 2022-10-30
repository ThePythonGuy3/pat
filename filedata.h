#ifndef FILEDATA
#define FILEDATA

template <typename TP>
std::time_t to_time_t(TP tp);

long long* readMetadata(std::filesystem::path path);
std::string readMetadataStr(std::filesystem::path path);

#endif

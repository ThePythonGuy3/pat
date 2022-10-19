#ifndef HIGH_ZLIB
#define HIGH_ZLIB

namespace highZlib
{
	extern std::string compress_string(const std::string &str);
	extern std::string compress_string(const std::string &str, int compressionlevel);
	extern std::string decompress_string(const std::string &str);
}

#endif
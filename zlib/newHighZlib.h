#ifndef NEW_HIGH_ZLIB
#define NEW_HIGH_ZLIB

namespace newHighZlib
{
	extern std::string compress(const std::string &str);
	extern std::string compress(const std::string &str, int compressionlevel);
	extern std::string decompress(const std::string &str);
}

#endif

#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstddef>
#include <string>

/* Magic string to identify whether stegged or not */
const std::string MAGIC_STRING = "#*";

/* Size of the BMP header copied / skipped as is */
constexpr std::size_t BMP_HEADER_SIZE = 54;

/* Longest secret file extension accepted (including the '.') */
constexpr std::size_t MAX_FILE_SUFFIX = 7;

/* Returns the extension of fname starting at the last '.', or "" if none */
inline std::string file_extension(const std::string &fname)
{
    auto pos = fname.rfind('.');
    return pos == std::string::npos ? "" : fname.substr(pos);
}

#endif

#ifndef SOUND_SPHERE_UTILS_STRING_HPP
#define SOUND_SPHERE_UTILS_STRING_HPP

#include <string>
#include <vector>

#if defined(_WIN32)
#include <Esent.h>
#endif

namespace soundsphere {

/**
 * @brief String vector.
 */
typedef std::vector<std::string> StringVec;

#if defined(_WIN32)

/**
 * @brief Wide string.
 * @note Windows only.
 */
typedef std::shared_ptr<WCHAR> wstring;

/**
 * @brief Convert wide string to UTF-8 string.
 * @note Windows only.
 * @param[in] src   Wide string.
 * @return UTF-8 string.
 */
std::string wide_to_utf8(WCHAR* src);

/**
 * @brief Convert UTF-8 string into wide string.
 * @note Windows only.
 * @param[in] src   UTF-8 string.
 * @return Wide string.
 */
wstring utf8_to_wide(const char* src);

#endif

/**
 * @brief Split string into tokens.
 * @param[in] orig  Source string.
 * @param[in] delimiter Delimiter.
 * @return Tokens.
 */
StringVec string_split(const std::string& orig, const std::string& delimiter);

/**
 * @brief Wildcard string.
 * @param[in] str       String to match.
 * @param[in] pattern   Wildcard string.
 * @return              true if match, otherwise not match.
 */
bool string_wildcard(const std::string& str, const std::string& pattern);

/**
 * @brief Remove leading and trailing whitespaces.
 * @param[in] str   The original string.
 * @return          The string that removing leading and trailing whitespaces.
 */
std::string string_trim(const std::string& str);

}

#endif

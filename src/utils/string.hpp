#ifndef SOUND_SPHERE_UTILS_STRING_HPP
#define SOUND_SPHERE_UTILS_STRING_HPP

#include <string>
#include <vector>

namespace soundsphere {

typedef std::vector<std::string> StringVec;

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

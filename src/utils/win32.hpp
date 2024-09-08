#ifndef SOUND_SPHERE_UTILS_WIN32_HPP
#define SOUND_SPHERE_UTILS_WIN32_HPP

#include <string>
#include <Esent.h>
#include <memory>

namespace soundsphere {

/**
 * @brief Wide string.
 */
typedef std::shared_ptr<WCHAR> wstring;

/**
 * @brief Convert wide string to UTF-8 string.
 */
std::string wide_to_utf8(WCHAR* src);

/**
 * @brief Convert UTF-8 string into wide string.
 */
wstring utf8_to_wide(const char* src);

}

#endif

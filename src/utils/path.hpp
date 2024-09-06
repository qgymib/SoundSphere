#ifndef SOUND_SPHERE_UTILS_PATH_HPP
#define SOUND_SPHERE_UTILS_PATH_HPP

#include <string>

namespace soundsphere {

/**
 * @brief Get filename.
 * @param[in] path  Full path.
 * @param[in] with_exit whether Keep extensions.
 * @return Filename.
 */
std::string basename(const std::string& path, bool with_ext = false);

}

#endif

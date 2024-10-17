#ifndef SOUND_SPHERE_UTILS_PATH_HPP
#define SOUND_SPHERE_UTILS_PATH_HPP

#include <string>

namespace soundsphere
{

/**
 * @brief Get base directory.
 * @param[in] path  Directory path.
 * @return Base directory path.
 */
std::string dirname(const std::string &path);

/**
 * @brief Get filename.
 * @param[in] path  Full path.
 * @param[in] with_exit whether Keep extensions.
 * @return Filename.
 */
std::string basename(const std::string &path, bool with_ext);

/**
 * @brief Get extension name.
 * @param[in] path  File path.
 * @return Extension name (with the leading dot).
 */
std::string extname(const std::string &path);

} // namespace soundsphere

#endif

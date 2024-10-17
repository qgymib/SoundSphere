#ifndef SOUND_SPHERE_UTILS_ENV_HPP
#define SOUND_SPHERE_UTILS_ENV_HPP

#include "utils/string.hpp"

namespace soundsphere
{

/**
 * @brief Gets a value from the current environment.
 * @param[in] name  The name of the environment.
 * @return          The value.
 */
std::string getenv(const std::string &name);

} // namespace soundsphere

#endif

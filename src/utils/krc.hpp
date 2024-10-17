#ifndef SOUND_SPHERE_UTILS_KRC_HPP
#define SOUND_SPHERE_UTILS_KRC_HPP

#include "utils/string.hpp"

namespace soundsphere
{

/**
 * @brief Convert krc to lyric.
 * @param[in] data  KRC data.
 * @return  Normal lyric.
 */
std::string krc_to_lyric(const std::string &data);

} // namespace soundsphere

#endif

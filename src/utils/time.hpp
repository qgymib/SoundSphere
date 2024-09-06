#ifndef SOUND_SPHERE_UTILS_TIME_HPP
#define SOUND_SPHERE_UTILS_TIME_HPP

#include <cstddef>

namespace soundsphere {

/**
 * @brief Convert seconds into HH:MM:SS.
 * @param[out] buff     Buffer.
 * @param[in] size      Buffer size.
 * @param[in] seconds   Seconds.
 */
void time_seconds_to_string(char* buff, size_t size, double seconds);

}

#endif

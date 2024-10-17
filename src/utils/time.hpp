#ifndef SOUND_SPHERE_UTILS_TIME_HPP
#define SOUND_SPHERE_UTILS_TIME_HPP

#include <cstddef>
#include <cstdint>
#include "utils/string.hpp"

namespace soundsphere
{

/**
 * @brief Convert seconds into HH:MM:SS.
 * @param[out] buff     Buffer.
 * @param[in] size      Buffer size.
 * @param[in] seconds   Seconds.
 */
void time_seconds_to_string(char *buff, size_t size, double seconds);

/**
 * @brief Convert seconds into HH:MM:SS.
 */
std::string time_seconds_to_string(double seconds);

/**
 * @brief Get current clock time in milliseconds.
 * @return Milliseconds.
 */
uint64_t clock_time_ms(void);

} // namespace soundsphere

#endif

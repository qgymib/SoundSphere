#ifndef SOUND_SPHERE_UTILS_DUMP_HPP
#define SOUND_SPHERE_UTILS_DUMP_HPP

#include <cstddef>

namespace soundsphere {

/**
 * @brief Write binary data to file.
 * @param[in] path  File path.
 * @param[in] data  Data.
 * @param[in] size  Data size.
 */
void dump(const char* path, const void* data, size_t size);

}

#endif

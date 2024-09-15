#ifndef SOUND_SPHERE_UTILS_BINARY_HPP
#define SOUND_SPHERE_UTILS_BINARY_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

namespace soundsphere {

typedef std::vector<uint8_t> Bin;
typedef std::vector<Bin> BinVec;

/**
 * @brief Write binary data to file.
 * @param[in] path  File path.
 * @param[in] data  Data.
 * @param[in] size  Data size.
 */
void dump(const char* path, const void* data, size_t size);

}

#endif

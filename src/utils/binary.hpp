#ifndef SOUND_SPHERE_UTILS_BINARY_HPP
#define SOUND_SPHERE_UTILS_BINARY_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <set>
#include <functional>

namespace soundsphere
{

typedef std::vector<uint8_t> Bin;
typedef std::vector<Bin> BinVec;

/**
 * @brief Write binary data to file.
 * @param[in] path  File path.
 * @param[in] data  Data.
 * @param[in] size  Data size.
 */
void dump(const char *path, const void *data, size_t size);

/**
 * @brief Remove duplicated items in vector.
 * @param[in] vec   The vector to check.
 * @param[in] fn    The field to check.
 * @return          The checked vector.
 */
template <typename T, typename K>
std::vector<T> remove_duplicate_r(const std::vector<T> &vec, std::function<K(const T &)> fn)
{
    std::vector<T> ret;
    std::set<K> seen;

    auto it = vec.begin();
    for (; it != vec.end(); it++)
    {
        const K key = fn(*it);
        if (seen.find(key) == seen.end())
        {
            ret.push_back(*it);
            seen.insert(*it);
        }
    }

    return ret;
}

/**
 * @brief Remove duplicated items in vector.
 * @param[in,out]   vec   The vector to check.
 * @param[in]       fn    The field to check.
 */
template <typename T, typename K> void remove_duplicate(std::vector<T> &vec, std::function<K(const T &)> fn)
{
    std::set<K> seen;
    auto it = vec.begin();
    while (it != vec.end())
    {
        const K v = fn(*it);
        if (seen.find(v) != seen.end())
        {
            it = vec.erase(it);
        }
        else
        {
            seen.insert(v);
            it++;
        }
    }
}

} // namespace soundsphere

#endif

#ifndef SOUND_SPHERE_UTILS_DEFINES_HPP
#define SOUND_SPHERE_UTILS_DEFINES_HPP

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x

/**
 * @brief Program name.
 */
#if defined(CMAKE_PROJECT_NAME)
#define PROJECT_NAME STRINGIFY(CMAKE_PROJECT_NAME)
#else
#define PROJECT_NAME "SoundSphere"
#endif

/**
 * @brief Program version.
 */
#if defined(CMAKE_PROJECT_VERSION)
#define PROJECT_VERSION STRINGIFY(CMAKE_PROJECT_VERSION)
#else
#define PROJECT_VERSION "unknown"
#endif

/**
 * @brief Program ID.
 */
#define PROJECT_ID "io.github.qgymib.SoundSphere"

/**
 * @brief Get array size.
 */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#endif

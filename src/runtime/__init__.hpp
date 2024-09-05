#ifndef SOUND_SPHERE_RUNTIME_INIT_HPP
#define SOUND_SPHERE_RUNTIME_INIT_HPP

namespace soundsphere {

typedef struct runtime
{
    int useless;
} runtime_t;

/**
 * @brief Global runtime.
 */
extern runtime_t _G;

/**
 * @brief Initialize runtime.
 */
void runtime_init(void);

/**
 * @brief Cleanup runtime.
 */
void runtime_exit(void);

}

#endif

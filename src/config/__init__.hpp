#ifndef SOUND_SPHERE_CONFIG_INIT_HPP
#define SOUND_SPHERE_CONFIG_INIT_HPP

#include "utils/string.hpp"

namespace soundsphere {

typedef struct config
{
    config();

    /**
     * @brief Time for auto-centering in milliseconds.
     */
    uint64_t    lyric_auto_center_time_ms;
} config_t;

/**
 * @brief Global configuration.
 */
extern config_t _config;

/**
 * @brief Initialize configuration.
 */
void config_init(void);

/**
 * @brief Cleanup configuration.
 */
void config_exit(void);

/**
 * @brief Save configuation.
 */
bool config_save(std::string& errinfo);

}

#endif

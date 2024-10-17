#ifndef SOUND_SPHERE_CONFIG_INIT_HPP
#define SOUND_SPHERE_CONFIG_INIT_HPP

#include "utils/string.hpp"

namespace soundsphere
{

typedef struct config_lyric
{
    config_lyric();

    /**
     * @brief Time for auto-centering in milliseconds.
     */
    uint64_t auto_center_time_ms;

    /**
     * @brief Background color of lyric font.
     */
    float back_font_color[4];

    /**
     * @brief Current shown lyric font color.
     */
    float fore_font_color[4];
} config_lyric_t;

typedef struct config
{
    config();

    /**
     * @brief Language
     */
    std::string language;

    /**
     * @brief Volume. [0, 100].
     */
    int volume;

    /**
     * @brief Lyric
     */
    config_lyric_t lyric;

    /**
     * @brief Song paths.
     */
    StringVec songs;

    /**
     * @brief Proxy.
     */
    std::string proxy;
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
bool config_save(std::string &errinfo);

} // namespace soundsphere

#endif

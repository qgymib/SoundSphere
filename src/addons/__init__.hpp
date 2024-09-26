#ifndef SOUND_SPHERE_ADDONS_INIT_HPP
#define SOUND_SPHERE_ADDONS_INIT_HPP

#include "SoundSphere/addon.h"

namespace soundsphere {

/**
 * @brief Initialize addon system.
 */
void addon_init(void);

/**
 * @brief Exit addon system.
 */
void addon_exit(void);

/**
 * @brief Draw addon.
 */
void addon_draw(void);

extern const SS_curl    export_com_curl;
extern const SS_LibC    export_com_libc;
extern const SS_ImGui   export_com_imgui;

}

#endif

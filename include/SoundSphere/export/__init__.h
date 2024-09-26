#ifndef SOUND_SPHERE_RE_EXPORT_INIT_H
#define SOUND_SPHERE_RE_EXPORT_INIT_H

#include "SoundSphere/export/curl.h"
#include "SoundSphere/export/imgui.h"
#include "SoundSphere/export/libc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ss_export_id
{
    /**
     * @see #SS_LIBC.
     */
    SS_EXPORT_LIBC  = 0,

    /**
     * @see #SS_ImGui.
     */
    SS_EXPORT_IMGUI = 1,

    /**
     * @see #SS_curl.
     */
    SS_EXPORT_CURL  = 2,
} ss_export_id_t;

#ifdef __cplusplus
}
#endif
#endif

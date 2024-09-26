#ifndef SOUND_SPHERE_ADDON_H
#define SOUND_SPHERE_ADDON_H

#include "SoundSphere/export/__init__.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef const void* ss_any_com_t;

typedef struct ss_addon_info
{
    /**
     * @brief Addon ID.
     */
    const char* id;

    /**
     * @brief Exit callback.
     *
     * Called when addon is going to unload.
     *
     * This function is called only once.
     */
    void (*exit)(void);

    /**
     * @brief Draw callback.
     *
     * This function is called every frame.
     *
     * @warning Do not perform any blocking operations in this function.
     */
    void (*draw)(void);
} ss_addon_info_t;

typedef struct ss_api
{
    /**
     * @brief Get export component API.
     *
     * SoundSphere re-export it self and some of dependency components as C API.
     *
     * An addon cannot directly use these original APIs because they may static-
     * link to SoundSphere. So to use these components, user should get the APIs
     * via this function first.
     *
     * @param[out] com  A pointer to store component handle address. It is managed
     *   by SoundSphere, it is available during addon's lifecycle and released after
     *   #sound_sphere_addon_info_t::exit() is called.
     * @param[in] id    Component ID.
     * @return When success, it returns the size of component handle. If failure,
     *   returns a negative value.
     */
    int (*com_export)(ss_any_com_t* com, ss_export_id_t id);
} ss_api_t;

/**
 * @brief Entry for addon.
 * @param[in,out] info  Addon information.
 * @param[in] api   The api object that SoundSphere exposed.
 * @return  0 if initialize success, otherwise failed.
 */
typedef int (*soundsphere_addon_init)(ss_addon_info_t* info, ss_api_t* api);

#ifdef __cplusplus
}
#endif
#endif

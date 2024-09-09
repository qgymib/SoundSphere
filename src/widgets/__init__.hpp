#ifndef SOUND_SPHERE_WIDGETS_INIT_HPP
#define SOUND_SPHERE_WIDGETS_INIT_HPP

#include <imgui.h>

namespace soundsphere {

typedef struct widget_pos_size
{
    ImVec2 pos;
    ImVec2 size;
} widget_pos_size_t;

typedef struct widget_layout
{
    widget_pos_size_t   filter;
    widget_pos_size_t   cover;
    widget_pos_size_t   lyric;
    widget_pos_size_t   playlist;
    widget_pos_size_t   playbar;
    widget_pos_size_t   statusbar;
} widget_layout_t;

typedef struct widget
{
    /**
     * @brief Initialize addon.
     */
    void (*init)(void);

    /**
     * @brief Cleanup addon.
     */
    void (*exit)(void);

    /**
     * @brief Draw widget UI.
     */
    void (*draw)(void);
} widget_t;

extern const widget_t dummy_player;
extern const widget_t menubar_about;
extern const widget_t menubar_open;
extern const widget_t menubar_preferences;
extern const widget_t menubar_translations;
extern const widget_t ui_cover;
extern const widget_t ui_filter;
extern const widget_t ui_lyric;
extern const widget_t ui_playbar;
extern const widget_t ui_playlist;
extern const widget_t ui_statusbar;

extern widget_layout_t _layout;

/**
 * @brief Initialize all addons.
 */
void widget_init(void);

/**
 * @brief Cleanup all addons.
 */
void widget_exit(void);

/**
 * @brief Draw all addons.
 */
void widget_draw(void);

}

#endif

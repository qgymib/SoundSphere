#ifndef SOUND_SPHERE_WIDGETS_INIT_HPP
#define SOUND_SPHERE_WIDGETS_INIT_HPP

#include "utils/imgui.hpp"

namespace soundsphere
{

typedef struct widget_pos_size
{
    ImVec2 pos;  /**< Position. */
    ImVec2 size; /**< Size. */
} widget_pos_size_t;

typedef struct widget_layout
{
    widget_pos_size_t filter;
    widget_pos_size_t cover;
    widget_pos_size_t lyric;
    widget_pos_size_t playlist;
    widget_pos_size_t playbar;
    widget_pos_size_t statusbar;
    widget_pos_size_t title;
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

/**
 * @brief Dummy widgets. Does not have UI.
 * @{
 */
extern const widget_t dummy_player; /**< The music player. */
/**
 * @}
 */

/**
 * @brief MenuBar widgets. Registered into MenuBar.
 * @{
 */
extern const widget_t menubar_about;
extern const widget_t menubar_debug;
extern const widget_t menubar_open;
extern const widget_t menubar_preferences;
extern const widget_t menubar_translations;
/**
 * @}
 */

/**
 * @brief UI widgets. Have fixed position on UI.
 * The position and size is defined in #_layout.
 * @{
 */
extern const widget_t ui_cover;
extern const widget_t ui_filter;
extern const widget_t ui_lyric;
extern const widget_t ui_playbar;
extern const widget_t ui_playlist;
extern const widget_t ui_statusbar;
extern const widget_t ui_title;
/**
 * @}
 */

/**
 * @brief Tool widgets. Can be opened by other widgets.
 * {
 */
extern const widget_t tool_tag_editor;
/**
 * @}
 */

/**
 * @brief UI layout for ui_* widgets.
 */
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

} // namespace soundsphere

#endif

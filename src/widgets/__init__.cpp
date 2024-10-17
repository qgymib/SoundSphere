#include <imgui.h>
#include "i18n/__init__.h"
#include "runtime/__init__.hpp"
#include "__init__.hpp"

soundsphere::widget_layout_t soundsphere::_layout;

static const soundsphere::widget_t *s_main_ui[] = {
    /* Menu items. Order is important. */
    &soundsphere::menubar_open,
    &soundsphere::menubar_preferences,
    &soundsphere::menubar_translations,
    &soundsphere::menubar_debug,
    &soundsphere::menubar_about,

    /* UI widgets. */
    &soundsphere::ui_cover,
    &soundsphere::ui_filter,
    &soundsphere::ui_lyric,
    &soundsphere::ui_playbar,
    &soundsphere::ui_playlist,
    &soundsphere::ui_statusbar,
    &soundsphere::ui_title,

    /* Dummy widgets. */
    &soundsphere::dummy_player,

    /* Tool widgets. */
    &soundsphere::tool_tag_editor,
};

static float _widget_get_main_menu_bar_height(void)
{
    float main_menu_bar_height = 0;
    if (ImGui::BeginMainMenuBar())
    {
        /* The height of MainMenuBar is can be get here. */
        main_menu_bar_height = ImGui::GetWindowHeight();
        ImGui::EndMainMenuBar();
    }
    return main_menu_bar_height;
}

static float _widget_get_cover_width(ImGuiIO &io)
{
    float cover_width_x = io.DisplaySize.x * 0.5f;
    float cover_width_y = io.DisplaySize.y * 0.5f;
    float cover_width = cover_width_x < cover_width_y ? cover_width_x : cover_width_y;
    return cover_width > 256.0f ? 256.0f : cover_width;
}

static float _widget_get_filter_width(ImGuiIO &io)
{
    float filter_width = io.DisplaySize.x * 0.3f;
    return filter_width > 256.0f ? 256.0f : filter_width;
}

void soundsphere::widget_init(void)
{
    for (size_t i = 0; i < IM_ARRAYSIZE(s_main_ui); i++)
    {
        const soundsphere::widget_t *entry = s_main_ui[i];
        entry->init();
    }
}

void soundsphere::widget_exit(void)
{
    for (size_t i = 0; i < IM_ARRAYSIZE(s_main_ui); i++)
    {
        const soundsphere::widget_t *entry = s_main_ui[i];
        entry->exit();
    }
}

static void _widget_draw_update_pos_size(void)
{
    ImGuiIO &io = ImGui::GetIO();

    const float main_menu_bar_height = _widget_get_main_menu_bar_height();
    const float status_bar_height = ImGui::GetFrameHeight() * 1.5f;
    const float play_bar_height = ImGui::GetFrameHeight() * 1.7f;
    const float filter_width = _widget_get_filter_width(io);
    const float cover_width = _widget_get_cover_width(io);
    const float title_height = 64.0f;

    soundsphere::_layout.statusbar.size.x = io.DisplaySize.x;
    soundsphere::_layout.statusbar.size.y = status_bar_height;
    soundsphere::_layout.statusbar.pos.x = 0;
    soundsphere::_layout.statusbar.pos.y = io.DisplaySize.y - status_bar_height;

    soundsphere::_layout.playbar.size.x = io.DisplaySize.x;
    soundsphere::_layout.playbar.size.y = play_bar_height;
    soundsphere::_layout.playbar.pos.x = 0;
    soundsphere::_layout.playbar.pos.y = io.DisplaySize.y - (status_bar_height + play_bar_height);

    soundsphere::_layout.filter.size.x = filter_width;
    soundsphere::_layout.filter.size.y =
        io.DisplaySize.y - (main_menu_bar_height + play_bar_height + status_bar_height);
    soundsphere::_layout.filter.pos.x = 0;
    soundsphere::_layout.filter.pos.y = main_menu_bar_height;

    soundsphere::_layout.cover.size.x = cover_width;
    soundsphere::_layout.cover.size.y = cover_width;
    soundsphere::_layout.cover.pos.x = io.DisplaySize.x - cover_width;
    soundsphere::_layout.cover.pos.y = main_menu_bar_height;

    soundsphere::_layout.title.size.x = cover_width;
    soundsphere::_layout.title.size.y = title_height;
    soundsphere::_layout.title.pos.x = io.DisplaySize.x - cover_width;
    soundsphere::_layout.title.pos.y = main_menu_bar_height + cover_width;

    soundsphere::_layout.lyric.size.x = cover_width;
    soundsphere::_layout.lyric.size.y =
        io.DisplaySize.y - (main_menu_bar_height + cover_width + title_height + status_bar_height + play_bar_height);
    soundsphere::_layout.lyric.pos.x = io.DisplaySize.x - cover_width;
    soundsphere::_layout.lyric.pos.y = main_menu_bar_height + cover_width + title_height;

    soundsphere::_layout.playlist.size.x = io.DisplaySize.x - filter_width - cover_width;
    soundsphere::_layout.playlist.size.y =
        io.DisplaySize.y - main_menu_bar_height - play_bar_height - status_bar_height;
    soundsphere::_layout.playlist.pos.x = filter_width;
    soundsphere::_layout.playlist.pos.y = main_menu_bar_height;
}

void soundsphere::widget_draw(void)
{
    _widget_draw_update_pos_size();

    for (size_t i = 0; i < IM_ARRAYSIZE(s_main_ui); i++)
    {
        const soundsphere::widget_t *widget = s_main_ui[i];
        widget->draw();
    }
}

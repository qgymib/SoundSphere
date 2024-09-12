#include <imgui.h>
#include "i18n/__init__.h"
#include "runtime/__init__.hpp"
#include "__init__.hpp"

static bool s_show_preferences_window = false;

static void _widget_preferences_init(void)
{
}

static void _widget_preferences_exit(void)
{
}

static void _widget_preferences_draw_select_lang(void)
{
    const char* item_locals[] = {
#define I18N_EXPAND_LOCALE_AS_STRING(a, b)   b.lang,
    I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_STRING)
#undef I18N_EXPAND_LOCALE_AS_STRING
    };
    static int item_type = 0;
    if (ImGui::Combo(soundsphere_i18n->localization, &item_type, item_locals, IM_ARRAYSIZE(item_locals)))
    {
        soundsphere_i18n_set_locale((soundsphere_i18n_locale_t)item_type);
    }
}

static void _widget_preferences_draw_lyric_auto_center_time(void)
{
    int time = (int)(soundsphere::_G.lyric.auto_center_time / 1000 / 1000 / 1000);
    if (ImGui::InputInt(soundsphere_i18n->lyric_auto_center_time, &time))
    {
        if (time >= 0)
        {
            soundsphere::_G.lyric.auto_center_time = (uint64_t)time * 1000 * 1000 * 1000;
        }
    }
    ImGui::SameLine();
    ImGui::TipMark(soundsphere_i18n->tip_lyric_auto_center_time);
}

static void _widget_preferences_draw(void)
{
    /* Register to MainMenu. */
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(soundsphere_i18n->settings))
        {
            ImGui::MenuItem(soundsphere_i18n->preferences, nullptr, &s_show_preferences_window);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    /* Show window. */
    if (s_show_preferences_window)
    {
        const char* window_name = soundsphere_i18n->preferences;
        if (ImGui::Begin(window_name, &s_show_preferences_window, ImGuiWindowFlags_AlwaysAutoResize))
        {
            _widget_preferences_draw_select_lang();
            _widget_preferences_draw_lyric_auto_center_time();
        }
        ImGui::End();
    }
}

const soundsphere::widget_t soundsphere::menubar_preferences = {
_widget_preferences_init,
_widget_preferences_exit,
_widget_preferences_draw,
};

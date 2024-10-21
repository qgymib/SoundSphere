#include <imgui.h>
#include "config/__init__.hpp"
#include "i18n/__init__.h"
#include "runtime/__init__.hpp"
#include "__init__.hpp"

typedef struct preference_tab
{
    /**
     * @brief The name of tab.
     */
    const char *name;

    /**
     * @brief Draw function.
     */
    void (*draw)(void);
} preference_tab_t;

typedef struct preferences_ctx
{
    preferences_ctx();

    /**
     * @brief Show preferences window.
     */
    bool show_window;

    /**
     * @brief Selected locale.
     */
    int selected_locale;
} preferences_ctx_t;

static preferences_ctx *s_preferences_ctx = nullptr;

///////////////////////////////////////////////////////////////////////////////
// Preference tab: Generic
///////////////////////////////////////////////////////////////////////////////

static void _widget_preferences_draw_generic_select_lang(void)
{
    const char *item_locals[] = {
#define I18N_EXPAND_LOCALE_AS_STRING(a) soundsphere_i18n_##a.translation->lang,
        I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_STRING)
#undef I18N_EXPAND_LOCALE_AS_STRING
    };
    static const char *item_lang[] = {
#define I18N_EXPAND_LOCALE_AS_STRING(a) soundsphere_i18n_##a.language_territory,
        I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_STRING)
#undef I18N_EXPAND_LOCALE_AS_STRING
    };
    const char *label = _T->localization;
    if (ImGui::Combo(label, &s_preferences_ctx->selected_locale, item_locals, IM_ARRAYSIZE(item_locals)))
    {
        soundsphere::_config.language = item_lang[s_preferences_ctx->selected_locale];
        soundsphere_i18n_reload_locale();
    }
}

static void _widget_preference_draw_generic(void)
{
    _widget_preferences_draw_generic_select_lang();
    ImGui::InputText(_T->proxy, &soundsphere::_config.proxy);
}

///////////////////////////////////////////////////////////////////////////////
// Preference tab: Lyric
///////////////////////////////////////////////////////////////////////////////

static void _widget_preferences_draw_lyric_auto_center_time(void)
{
    int time = (int)(soundsphere::_config.lyric.auto_center_time_ms / 1000);
    const char *label = _T->lyric_auto_center_time;
    if (ImGui::InputInt(label, &time))
    {
        if (time >= 0)
        {
            soundsphere::_config.lyric.auto_center_time_ms = (uint64_t)time * 1000;
        }
    }
    ImGui::SameLine();
    ImGui::TipMark(_T->tip_lyric_auto_center_time);
}

static void _widget_preferences_draw_lyric_color(void)
{
    int flags = 0;
    ImGui::ColorEdit4(_T->lyric_font_fore_color, soundsphere::_config.lyric.fore_font_color, flags);
    ImGui::ColorEdit4(_T->lyric_font_back_color, soundsphere::_config.lyric.back_font_color, flags);
}

static void _widget_preference_draw_lyric(void)
{
    _widget_preferences_draw_lyric_auto_center_time();
    _widget_preferences_draw_lyric_color();
}

preferences_ctx::preferences_ctx()
{
    show_window = false;
    selected_locale = (int)soundsphere_i18n->locale;
}

static void _widget_preferences_init(void)
{
    s_preferences_ctx = new preferences_ctx_t;
}

static void _widget_preferences_exit(void)
{
    delete s_preferences_ctx;
    s_preferences_ctx = nullptr;
}

static void _widget_preferences_draw(void)
{
    /* Register to MainMenu. */
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(_T->settings))
        {
            ImGui::MenuItem(_T->preferences, nullptr, &s_preferences_ctx->show_window);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    if (!s_preferences_ctx->show_window)
    {
        return;
    }

    /* Preferences tabs. */
    const preference_tab_t tabs[] = {
        { _T->generic, _widget_preference_draw_generic },
        { _T->lyric,   _widget_preference_draw_lyric   },
    };

    /* Show window. */
    const char *window_name = _T->preferences;
    const int window_flag = ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin(window_name, &s_preferences_ctx->show_window, window_flag))
    {
        if (ImGui::BeginTabBar("preferences_tabbar"))
        {
            for (size_t i = 0; i < IM_ARRAYSIZE(tabs); i++)
            {
                const preference_tab_t *tab = &tabs[i];
                if (ImGui::BeginTabItem(tab->name))
                {
                    tab->draw();
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::menubar_preferences = {
    _widget_preferences_init,
    _widget_preferences_exit,
    _widget_preferences_draw,
    nullptr,
};

#include "i18n/__init__.h"
#include "__init__.hpp"

static const char* s_locales[] = {
#define I18N_EXPAND_LOCALE_AS_STRING(a, b)   b.lang,
    I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_STRING)
#undef I18N_EXPAND_LOCALE_AS_STRING
};

static const char* s_translation_key[] = {
#define EXPAND_STRING_AS_NAME(s)    #s,
    I18N_STRING_TABLE(EXPAND_STRING_AS_NAME)
#undef EXPAND_STRING_AS_NAME
};

/**
 * @brief Should we show the translation window.
 */
static bool s_menu_show_translations = false;

/**
 * @brief Translation percentage of selected locale.
 */
static float s_translation_percentage = 0.0f;

/**
 * @brief Selected locale.
 */
static soundsphere_i18n_locale_t s_selected_locale_id = soundsphere_i18n_locale_t::I18N_LOCALE_EN_US;

/**
 * @brief Selected locale.
 * Everything in this window (except window title) use it's own locale.
 */
static soundsphere_i18n_t* s_selected_locale = nullptr;

/**
 * @brief Calcualte translation percentage.
 * @note en_US always has 100% translation percentage.
 * @param[in] locale    The locale
 * @return Translation percentage.
 */
static float _menubar_translation_calcuate_translation_percentage(soundsphere_i18n_locale_t locale)
{
    size_t translated_cnt = 0;
    size_t total_cnt = (size_t)soundsphere_i18n_string_t::I18N_STRING__MAX;

    if (locale == soundsphere_i18n_locale_t::I18N_LOCALE_EN_US)
    {
        return 1.0f;
    }

    soundsphere_i18n_t* original_locale = &soundsphere_i18n_en_us;
    soundsphere_i18n_t* target_locale = soundsphere_i18n_get_locale(locale);

    for (size_t i = 0; i < total_cnt; i++)
    {
        const char* original_text = soundsphere_i18n_locale_string(original_locale, (soundsphere_i18n_string_t)i);
        const char* translated_text = soundsphere_i18n_locale_string(target_locale, (soundsphere_i18n_string_t)i);
        if (original_text != translated_text)
        {
            translated_cnt++;
        }
    }

    return (float)translated_cnt / (float)total_cnt;
}

static void _menubar_translations_init(void)
{
    s_selected_locale = soundsphere_i18n_get_locale(s_selected_locale_id);
    s_translation_percentage = _menubar_translation_calcuate_translation_percentage(soundsphere_i18n_locale_t::I18N_LOCALE_EN_US);
}

static void _menubar_translations_exit(void)
{
}

/**
 * @brief Show tip
 */
static void _menubar_translations_tip(const char* str)
{
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(::ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(str);
        ImGui::PopTextWrapPos();

        ImGui::EndTooltip();
    }
}

static void _menubar_translations_show(void)
{
    const char* window_title = soundsphere_i18n->translations;
    int selected_locale = (int)s_selected_locale_id;

    if (!ImGui::Begin(window_title, &s_menu_show_translations, 0))
    {
        goto finish;
    }

    if (ImGui::Combo(s_selected_locale->localization, &selected_locale, s_locales, IM_ARRAYSIZE(s_locales)))
    {
        /* Update selected locale information. */
        s_selected_locale_id = (soundsphere_i18n_locale_t)selected_locale;
        s_selected_locale = soundsphere_i18n_get_locale(s_selected_locale_id);
        s_translation_percentage = _menubar_translation_calcuate_translation_percentage(s_selected_locale_id);
    }

    /* Show translation process bar. */
    ImGui::ProgressBar(s_translation_percentage);

    /* Show translation details table. */
    if (ImGui::BeginTable("about_translations_locale", 3, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 128);
        ImGui::TableSetupColumn(s_selected_locale->original_text, ImGuiTableColumnFlags_WidthFixed, 256);
        ImGui::TableSetupColumn(s_selected_locale->translated_text, ImGuiTableColumnFlags_WidthFixed, 256);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int)soundsphere_i18n_string_t::I18N_STRING__MAX);

        while (clipper.Step())
        {
            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
            {
                ImGui::PushID((void*)&s_translation_key[row_n]);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                const char* key = s_translation_key[row_n];
                ImGui::Text("%s", key);
                _menubar_translations_tip(key);

                ImGui::TableSetColumnIndex(1);
                const char* original_text = soundsphere_i18n_locale_string(&soundsphere_i18n_en_us, (soundsphere_i18n_string_t)row_n);
                ImGui::Text("%s", original_text);
                _menubar_translations_tip(original_text);

                ImGui::TableSetColumnIndex(2);
                const char* translated_text = soundsphere_i18n_locale_string(s_selected_locale, (soundsphere_i18n_string_t)row_n);
                translated_text = (translated_text == original_text) ? "" : translated_text;
                ImGui::Text("%s", translated_text);
                _menubar_translations_tip(translated_text);

                ImGui::PopID();
            }
        }

        ImGui::EndTable();
    }

finish:
    ImGui::End();
}

static void _menubar_translations_draw(void)
{
    /* Register menu. */
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(soundsphere_i18n->help))
        {
            ImGui::MenuItem(soundsphere_i18n->translations, nullptr, &s_menu_show_translations);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    /* Show window. */
    if (s_menu_show_translations)
    {
        _menubar_translations_show();
    }
}

const soundsphere::widget_t soundsphere::menubar_translations = {
_menubar_translations_init,
_menubar_translations_exit,
_menubar_translations_draw,
};

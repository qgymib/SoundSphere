#include "i18n/__init__.h"
#include "__init__.hpp"

static const char *s_locales[] = {
#define I18N_EXPAND_LOCALE_AS_STRING(a) soundsphere_i18n_##a.translation->lang,
    I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_STRING)
#undef I18N_EXPAND_LOCALE_AS_STRING
};

static const char *s_translation_key[] = {
#define EXPAND_STRING_AS_NAME(s) #s,
    I18N_STRING_TABLE(EXPAND_STRING_AS_NAME)
#undef EXPAND_STRING_AS_NAME
};

typedef struct menubar_translations_ctx
{
    menubar_translations_ctx();

    /**
     * @brief The default window size.
     */
    ImVec2 default_window_sz;

    /**
     * @brief Selected locale.
     */
    soundsphere_i18n_locale_t selected_locale_id;

    /**
     * @brief Selected locale.
     * Everything in this window (except window title) use it's own locale.
     */
    const soundsphere_i18n_t *selected_locale;

    /**
     * @brief Translation percentage of selected locale.
     */
    float translation_percentage;

    /**
     * @brief Should we show the translation window.
     */
    bool menu_show_translations;
} menubar_translations_ctx_t;

static menubar_translations_ctx_t *s_translation_ctx = nullptr;

/**
 * @brief Calculate translation percentage.
 * @note en_US always has 100% translation percentage.
 * @param[in] locale    The locale
 * @return Translation percentage.
 */
static float _menubar_translation_calcuate_translation_percentage(soundsphere_i18n_locale_t locale)
{
    size_t translated_cnt = 0;
    size_t total_cnt = (size_t)soundsphere_i18n_string_t::I18N_STRING__MAX;

    if (locale == soundsphere_i18n_locale_t::I18N_LOCALE_en_US)
    {
        return 1.0f;
    }

    const soundsphere_i18n_t *original_locale = &soundsphere_i18n_en_US;
    const soundsphere_i18n_t *target_locale = soundsphere_i18n_get_locale(locale);

    for (size_t i = 0; i < total_cnt; i++)
    {
        const char *original_text = soundsphere_i18n_locale_string(original_locale, (soundsphere_i18n_string_t)i);
        const char *translated_text = soundsphere_i18n_locale_string(target_locale, (soundsphere_i18n_string_t)i);
        if (original_text != translated_text)
        {
            translated_cnt++;
        }
    }

    return (float)translated_cnt / (float)total_cnt;
}

menubar_translations_ctx::menubar_translations_ctx()
{
    default_window_sz = ImVec2(640, 400);
    selected_locale_id = soundsphere_i18n_locale_t::I18N_LOCALE_en_US;
    selected_locale = soundsphere_i18n_get_locale(selected_locale_id);
    translation_percentage = _menubar_translation_calcuate_translation_percentage(selected_locale_id);
    menu_show_translations = false;
}

static void _menubar_translations_init(void)
{
    s_translation_ctx = new menubar_translations_ctx_t;
}

static void _menubar_translations_exit(void)
{
    delete s_translation_ctx;
    s_translation_ctx = nullptr;
}

static void _menubar_translations_show(void)
{
    int selected_locale = (int)s_translation_ctx->selected_locale_id;
    const char *label = s_translation_ctx->selected_locale->translation->localization;
    if (ImGui::Combo(label, &selected_locale, s_locales, IM_ARRAYSIZE(s_locales)))
    {
        /* Update selected locale information. */
        s_translation_ctx->selected_locale_id = (soundsphere_i18n_locale_t)selected_locale;
        s_translation_ctx->selected_locale = soundsphere_i18n_get_locale(s_translation_ctx->selected_locale_id);
        s_translation_ctx->translation_percentage =
            _menubar_translation_calcuate_translation_percentage(s_translation_ctx->selected_locale_id);
    }

    /* Show translation process bar. */
    ImGui::ProgressBar(s_translation_ctx->translation_percentage);

    /* Show translation details table. */
    if (ImGui::BeginTable("about_translations_locale", 3, ImGuiTableFlags_Borders))
    {
        label = "id";
        ImGui::TableSetupColumn(label, ImGuiTableColumnFlags_WidthFixed, 128);
        label = s_translation_ctx->selected_locale->translation->original_text;
        ImGui::TableSetupColumn(label, ImGuiTableColumnFlags_WidthFixed, 256);
        label = s_translation_ctx->selected_locale->translation->translated_text;
        ImGui::TableSetupColumn(label, ImGuiTableColumnFlags_WidthFixed, 256);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int)soundsphere_i18n_string_t::I18N_STRING__MAX);

        while (clipper.Step())
        {
            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
            {
                ImGui::PushID((void *)&s_translation_key[row_n]);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                const char *key = s_translation_key[row_n];
                ImGui::Text("%s", key);
                ImGui::Tip(key);

                ImGui::TableSetColumnIndex(1);
                const char *original_text =
                    soundsphere_i18n_locale_string(&soundsphere_i18n_en_US, (soundsphere_i18n_string_t)row_n);
                ImGui::Text("%s", original_text);
                ImGui::Tip(original_text);

                ImGui::TableSetColumnIndex(2);
                const char *translated_text = soundsphere_i18n_locale_string(s_translation_ctx->selected_locale,
                                                                             (soundsphere_i18n_string_t)row_n);
                translated_text = (translated_text == original_text) ? "" : translated_text;
                ImGui::Text("%s", translated_text);
                ImGui::Tip(translated_text);

                ImGui::PopID();
            }
        }

        ImGui::EndTable();
    }
}

static void _menubar_translations_draw(void)
{
    /* Register menu. */
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(_T->help))
        {
            ImGui::MenuItem(_T->translations, nullptr, &s_translation_ctx->menu_show_translations);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    /* Show window. */
    if (s_translation_ctx->menu_show_translations)
    {
        const char *window_title = _T->translations;
        ImGui::SetNextWindowSize(s_translation_ctx->default_window_sz, ImGuiCond_FirstUseEver);
        if (ImGui::Begin(window_title, &s_translation_ctx->menu_show_translations, 0))
        {
            _menubar_translations_show();
        }
        ImGui::End();
    }
}

const soundsphere::widget_t soundsphere::menubar_translations = {
    _menubar_translations_init,
    _menubar_translations_exit,
    _menubar_translations_draw,
};

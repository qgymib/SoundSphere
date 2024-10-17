#ifndef SOUND_SPHERE_I18N_INIT_H
#define SOUND_SPHERE_I18N_INIT_H
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief i18n strings.
 */
#define I18N_STRING_TABLE(xx)                                                                                          \
    xx(add) xx(add_folder) xx(about) xx(about_show_config_info) xx(artist) xx(bit_rate) xx(channel) xx(debug)          \
        xx(duration) xx(file) xx(generic) xx(help) xx(homepage) xx(lang) xx(localization) xx(lyric)                    \
            xx(lyric_auto_center_time) xx(lyric_font_back_color) xx(lyric_font_fore_color) xx(name) xx(open)           \
                xx(open_folder) xx(original_text) xx(path) xx(preferences) xx(proxy) xx(sample_rate) xx(save)          \
                    xx(search_artist) xx(search_lyric) xx(search_playlist) xx(search_title) xx(settings)               \
                        xx(tag_editor) xx(tip_lyric_auto_center_time) xx(title) xx(translated_text) xx(translations)   \
                            xx(version)

/**
 * @brief i18n locals.
 * @note To implement a new local, use #I18N_LOCALE_BEG and #I18N_LOCALE_END
 *   to disable struct initialize warning.
 */
#define I18N_LOCALE_TABLE(xx) xx(en_US) xx(zh_CN)

#define I18N_LOCALE_IMPL(lang, addr)                                                                                   \
    const soundsphere_i18n_t soundsphere_i18n_##lang = { I18N_LOCALE_##lang, #lang, addr }

/**
 * @brief Setup before localization.
 */
#if defined(__GNUC__)
#define I18N_LOCALE_BEG                                                                                                \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")
#else
#define I18N_LOCALE_BEG
#endif

/**
 * @brief Cleanup after localization.
 */
#if defined(__GNUC__)
#define I18N_LOCALE_END _Pragma("GCC diagnostic pop")
#else
#define I18N_LOCALE_END
#endif

/**
 * @brief The locale list.
 */
typedef enum soundsphere_i18n_locale_e
{
#define I18N_EXPAND_LOCALE_AS_ENUM(a) I18N_LOCALE_##a,
    I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_ENUM)
#undef I18N_EXPAND_LOCALE_AS_ENUM

    I18N_LOCALE__MAX,
    I18N_LOCALE__MIN = 0,
} soundsphere_i18n_locale_t;

/**
 * @brief String ID that has locales.
 */
typedef enum soundsphere_i18n_string_e
{
#define I18N_EXPAND_AS_ENUM(a) I18N_STRING_##a,
    I18N_STRING_TABLE(I18N_EXPAND_AS_ENUM)
#undef I18N_EXPAND_AS_ENUM

    I18N_STRING__MAX,
} soundsphere_i18n_string_t;

/**
 * @brief Strings that has locales.
 */
typedef struct soundsphere_i18n_translation
{
#define I18N_EXPAND_STRING_AS_FIELD(a) const char *a;
    I18N_STRING_TABLE(I18N_EXPAND_STRING_AS_FIELD)
#undef I18N_EXPAND_STRING_AS_FIELD
} soundsphere_i18n_translation_t;

typedef struct soundsphere_i18n_s
{
    /**
     * @brief Locale.
     */
    soundsphere_i18n_locale_t locale;

    /**
     * @brief The locale category for native language.
     * @see [Locale](https://wiki.archlinux.org/title/Locale)
     */
    const char *language_territory;

    /**
     * @brief Translations.
     */
    soundsphere_i18n_translation_t *translation;
} soundsphere_i18n_t;

#define I18N_EXPAND_LOCALE_AS_EXTERN(a) extern const soundsphere_i18n_t soundsphere_i18n_##a;
I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_EXTERN)
#undef I18N_EXPAND_LOCALE_AS_EXTERN

/**
 * @brief Global i18n.
 */
extern const soundsphere_i18n_t *soundsphere_i18n;

/**
 * @brief Global translation.
 * Fast access for #soundsphere_i18n->translation.
 */
extern const soundsphere_i18n_translation_t *_T;

/**
 * @brief Initialize i18n.
 */
void soundsphere_i18n_init(void);

/**
 * @brief Cleanup i18n.
 */
void soundsphere_i18n_exit(void);

/**
 * @brief Set current locale.
 * @param[in] locale    Current locale.
 */
void soundsphere_i18n_reload_locale(void);

/**
 * @brief Get locale translations.
 * @param[in] locale    Locale.
 * @return Translated strings.
 */
const soundsphere_i18n_t *soundsphere_i18n_get_locale(soundsphere_i18n_locale_t locale);

/**
 * @brief Get local string for id \p s.
 * @param[in] s The string id.
 * @return      The local string.
 */
const char *soundsphere_i18n_locale_string(const soundsphere_i18n_t *locale, soundsphere_i18n_string_t s);

#ifdef __cplusplus
}
#endif
#endif

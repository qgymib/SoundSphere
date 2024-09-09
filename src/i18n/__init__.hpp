#ifndef SOUND_SPHERE_I18N_INIT_HPP
#define SOUND_SPHERE_I18N_INIT_HPP

/**
 * @brief i18n strings.
 */
#define I18N_STRING_TABLE(xx)   \
    xx(about)                   \
    xx(about_show_config_info)  \
    xx(artist)                  \
    xx(file)                    \
    xx(help)                    \
    xx(homepage)                \
    xx(lang)                    \
    xx(localization)            \
    xx(name)                    \
    xx(open)                    \
    xx(open_folder)             \
    xx(original_text)           \
    xx(preferences)             \
    xx(search_in_playlist)      \
    xx(settings)                \
    xx(title)                   \
    xx(translated_text)         \
    xx(translations)            \
    xx(version)

/**
 * @brief i18n locals.
 * @note To implement a new local, use #I18N_LOCALE_BEG and #I18N_LOCALE_END
 *   to disable struct initialize warning.
 */
#define I18N_LOCALE_TABLE(xx)           \
    xx(I18N_EN_US,  i18n_en_us)         \
    xx(I18N_ZH_CN,  i18n_zh_cn)

/**
 * @brief Setup before localization.
 */
#if defined(__GNUC__)
#   define I18N_LOCALE_BEG  \
        _Pragma ("GCC diagnostic push") \
        _Pragma ("GCC diagnostic ignored \"-Wmissing-field-initializers\"")
#else
#   define I18N_LOCALE_BEG
#endif

/**
 * @brief Cleanup after localization.
 */
#if defined(__GNUC__)
#   define I18N_LOCALE_END  \
        _Pragma ("GCC diagnostic pop")
#else
#   define I18N_LOCALE_END
#endif

namespace soundsphere {

/**
 * @brief The locale list.
 */
typedef enum class i18n_locale_e
{
#define I18N_EXPAND_LOCALE_AS_ENUM(a, b) a,
    I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_ENUM)
#undef I18N_EXPAND_LOCALE_AS_ENUM

    I18N__MAX,
    I18N__MIN = 0,
} i18n_locale_t;

/**
 * @brief String ID that has locales.
 */
typedef enum class i18n_string_e
{
#define I18N_EXPAND_AS_ENUM(a)   a,
    I18N_STRING_TABLE(I18N_EXPAND_AS_ENUM)
#undef I18N_EXPAND_AS_ENUM

    I18N_STRING__MAX,
} i18n_string_t;

/**
 * @brief Strings that has locales.
 */
typedef struct i18n_s
{
#define I18N_EXPAND_STRING_AS_FIELD(a)  const char* a;
    I18N_STRING_TABLE(I18N_EXPAND_STRING_AS_FIELD)
#undef I18N_EXPAND_STRING_AS_FIELD
} i18n_t;

extern i18n_t* _i18n;

#define I18N_EXPAND_LOCALE_AS_EXTERN(a, b) extern i18n_t b;
I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_EXTERN)
#undef I18N_EXPAND_LOCALE_AS_EXTERN

/**
 * @brief Initialize i18n.
 */
void i18n_init(void);

/**
 * @brief Cleanup i18n.
 */
void i18n_exit(void);

/**
 * @brief Set current locale.
 * @param[in] locale    Current locale.
 */
void i18n_set_locale(i18n_locale_t locale);

/**
 * @brief Get local string for id \p s.
 * @param[in] s The string id.
 * @return      The local string.
 */
const char* i18n_locale_string(i18n_t* locale, i18n_string_t s);

/**
 * @brief Get locale translations.
 * @param[in] locale    Locale.
 * @return Translated strings.
 */
i18n_t* i18n_get_locale(i18n_locale_t locale);

}

#endif

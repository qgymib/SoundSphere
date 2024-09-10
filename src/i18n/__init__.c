#include <stdlib.h>
#include "__init__.h"

/**
 * @brief Language register order must match with #i18n_type_t.
 */
static soundsphere_i18n_t* s_i18n[] = {
#define I18N_EXPAND_LOCALE_AS_ARRAY(a, b)    &b,
    I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_ARRAY)
#undef I18N_EXPAND_LOCALE_AS_ARRAY
};

/**
 * @brief By default we are using en_US.
 */
soundsphere_i18n_t* soundsphere_i18n = &soundsphere_i18n_en_us;

void soundsphere_i18n_init()
{
#define I18N_EXPAND_AS_CHECK(A)  \
    do {\
        if (item->A == NULL) {\
            item->A = soundsphere_i18n_en_us.A;\
        }\
    } while (0);

    for (size_t i = 0; i < I18N_LOCALE__MAX; i++)
    {
        soundsphere_i18n_t* item = s_i18n[i];
        I18N_STRING_TABLE(I18N_EXPAND_AS_CHECK)
    }

#undef I18N_EXPAND_AS_CHECK
}

void soundsphere_i18n_exit(void)
{
}

const char* soundsphere_i18n_locale_string(soundsphere_i18n_t* locale,
    soundsphere_i18n_string_t s)
{
#define I18N_EXPAND_AS_CHOOSE(a) \
    case I18N_STRING_##a: return locale->a;

    switch(s)
    {
    I18N_STRING_TABLE(I18N_EXPAND_AS_CHOOSE)
    default:
        abort();
    }

#undef I18N_EXPAND_AS_CHOOSE
}

void soundsphere_i18n_set_locale(soundsphere_i18n_locale_t locale)
{
    soundsphere_i18n = soundsphere_i18n_get_locale(locale);
}

soundsphere_i18n_t* soundsphere_i18n_get_locale(soundsphere_i18n_locale_t locale)
{
    return s_i18n[(int)locale];
}

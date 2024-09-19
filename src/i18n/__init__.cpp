#include <spdlog/spdlog.h>
#include "config/__init__.hpp"
#include "utils/defines.hpp"
#include "utils/env.hpp"
#include "utils/string.hpp"
#include "__init__.h"

/**
 * @brief Language register order must match with #i18n_type_t.
 */
static const soundsphere_i18n_t* s_i18n[] = {
#define I18N_EXPAND_LOCALE_AS_ARRAY(a)    &soundsphere_i18n_##a,
    I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_ARRAY)
#undef I18N_EXPAND_LOCALE_AS_ARRAY
};

/**
 * @brief By default we are using en_US.
 */
const soundsphere_i18n_t* soundsphere_i18n = nullptr;

static void _soundsphere_i18n_init_locales(void)
{
#define I18N_EXPAND_AS_CHECK(A)  \
    do {\
        if (item->translation->A == NULL) {\
            item->translation->A = soundsphere_i18n_en_US.translation->A;\
        }\
    } while (0);

    for (size_t i = 0; i < I18N_LOCALE__MAX; i++)
    {
        const soundsphere_i18n_t* item = s_i18n[i];
        I18N_STRING_TABLE(I18N_EXPAND_AS_CHECK)
    }

#undef I18N_EXPAND_AS_CHECK
}

void soundsphere_i18n_init()
{
    _soundsphere_i18n_init_locales();
    soundsphere_i18n_reload_locale();
}

void soundsphere_i18n_exit(void)
{
}

const char* soundsphere_i18n_locale_string(const soundsphere_i18n_t* locale,
    soundsphere_i18n_string_t s)
{
#define I18N_EXPAND_AS_CHOOSE(a) \
    case I18N_STRING_##a: return locale->translation->a;

    switch(s)
    {
    I18N_STRING_TABLE(I18N_EXPAND_AS_CHOOSE)
    default:
        abort();
    }

#undef I18N_EXPAND_AS_CHOOSE
}

void soundsphere_i18n_reload_locale(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(s_i18n); i++)
    {
        const soundsphere_i18n_t* i18n = s_i18n[i];
        if (soundsphere::_config.language == i18n->language_territory)
        {
            soundsphere_i18n = i18n;
            return;
        }
    }

    /* No locale match, set to default. */
    soundsphere_i18n = &soundsphere_i18n_en_US;
}

const soundsphere_i18n_t* soundsphere_i18n_get_locale(soundsphere_i18n_locale_t locale)
{
    return s_i18n[(int)locale];
}

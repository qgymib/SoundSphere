#include <imgui.h>
#include <cstdlib>
#include "__init__.hpp"

/**
 * @brief Language register order must match with #i18n_type_t.
 */
static soundsphere::i18n_t* s_i18n[] = {
#define I18N_EXPAND_LOCALE_AS_ARRAY(a, b)    &soundsphere::b,
    I18N_LOCALE_TABLE(I18N_EXPAND_LOCALE_AS_ARRAY)
#undef I18N_EXPAND_LOCALE_AS_ARRAY
};

/**
 * @brief By default we are using en_US.
 */
soundsphere::i18n_t* soundsphere::_i18n = &soundsphere::i18n_en_us;

void soundsphere::i18n_init()
{
#define I18N_EXPAND_AS_CHECK(A)  \
    do {\
        if (item->A == NULL) {\
            item->A = soundsphere::i18n_en_us.A;\
        }\
    } while (0);

    for (size_t i = 0; i < IM_ARRAYSIZE(s_i18n); i++)
    {
        soundsphere::i18n_t* item = s_i18n[i];
        I18N_STRING_TABLE(I18N_EXPAND_AS_CHECK)
    }

#undef I18N_EXPAND_AS_CHECK
}

void soundsphere::i18n_exit(void)
{
}

const char* soundsphere::i18n_locale_string(soundsphere::i18n_string_t s)
{
#define I18N_EXPAND_AS_CHOOSE(a) \
    case soundsphere::i18n_string_t::a: return soundsphere::_i18n->a;

    switch(s)
    {
    I18N_STRING_TABLE(I18N_EXPAND_AS_CHOOSE)
    default:
        abort();
    }

#undef I18N_EXPAND_AS_CHOOSE
}

void soundsphere::i18n_set_locale(soundsphere::i18n_locale_t locale)
{
    soundsphere::_i18n = s_i18n[(int)locale];
}

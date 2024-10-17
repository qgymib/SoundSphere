#include <cstdlib>
#include "env.hpp"

std::string soundsphere::getenv(const std::string &name)
{
#if defined(_WIN32)
    soundsphere::wstring name_w = soundsphere::utf8_to_wide(name.c_str());

    wchar_t *buffer = nullptr;
    size_t number_of_elements = 0;
    errno_t errcode = _wdupenv_s(&buffer, &number_of_elements, name_w.get());
    if (errcode != 0 || buffer == NULL)
    {
        return "";
    }

    std::string ret = soundsphere::wide_to_utf8(buffer);
    free(buffer);

    return ret;
#else
    const char *env = std::getenv(name.c_str());
    return env != nullptr ? env : "";
#endif
}

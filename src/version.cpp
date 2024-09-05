#include "version.hpp"
#include "utils/defines.hpp"

#define SOUND_SPHERE_VERSION_STR     \
    STRINGIFY(SOUND_SPHERE_VERSION_MAJOR) "." STRINGIFY(SOUND_SPHERE_VERSION_MINOR) "." STRINGIFY(SOUND_SPHERE_VERSION_PATCH)

const char* soundsphere::version(void)
{
    return SOUND_SPHERE_VERSION_STR;
}

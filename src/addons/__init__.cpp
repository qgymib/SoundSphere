#include <ev.h>
#include <cassert>
#include <memory>
#include <map>
#include "utils/defines.hpp"
#include "utils/explorer.hpp"
#include "utils/path.hpp"
#include "__init__.hpp"

typedef struct export_component
{
    ss_export_id_t  id;
    const void*     addr;
    size_t          size;
} export_component_t;

typedef struct sound_sphere_api_impl
{
    ss_addon_info_t info;
    ss_api_t        api;
    ev_shdlib_t     lib;
} sound_sphere_api_impl_t;
static_assert(std::is_pod<sound_sphere_api_impl_t>::value);

typedef std::shared_ptr<sound_sphere_api_impl_t> AddonPtr;
typedef std::map<std::string, AddonPtr> AddonMap;

typedef struct addon_ctx
{
    addon_ctx();

    AddonMap    addon_map;
} addon_ctx_t;

static addon_ctx_t* s_addon_ctx = nullptr;

#define EXPORT_COMPONENT(x) &x, sizeof(x)

static const export_component_t s_export_components[] = {
    { SS_EXPORT_LIBC,   EXPORT_COMPONENT(soundsphere::export_com_libc) },
    { SS_EXPORT_IMGUI,  EXPORT_COMPONENT(soundsphere::export_com_imgui) },
    { SS_EXPORT_CURL,   EXPORT_COMPONENT(soundsphere::export_com_curl) },
};

addon_ctx::addon_ctx()
{
}

static int _com_export(ss_any_com_t* com, ss_export_id_t id)
{
    if (com == nullptr)
    {
        return -EINVAL;
    }

    if (id >= ARRAY_SIZE(s_export_components))
    {
        return -ENOSYS;
    }

    const export_component_t* comp = &s_export_components[id];
    assert(comp->id == id);

    *com = comp->addr;
    return (int)comp->size;
}

static void _release_addon(sound_sphere_api_impl_t* addon)
{
    if (addon->info.exit != nullptr)
    {
        addon->info.exit();
    }

    if (addon->lib.handle != EV_OS_SHDLIB_INVALID)
    {
        ev_dlclose(&addon->lib);
        addon->lib.handle = EV_OS_SHDLIB_INVALID;
    }
    delete addon;
}

static AddonPtr _new_addon_instance(void)
{
    sound_sphere_api_impl_t* impl = new sound_sphere_api_impl_t;

    memset(impl, 0, sizeof(*impl));
    impl->lib.handle = EV_OS_SHDLIB_INVALID;
    impl->api.com_export = _com_export;

    return AddonPtr(impl, _release_addon);
}

void soundsphere::addon_init(void)
{
    s_addon_ctx = new addon_ctx_t;

    std::string exepath = soundsphere::exepath();
    std::string exedir = soundsphere::dirname(exepath);

    static const char* filters[] = {
        "Plugins\n*.dll\n*.so",
    };
    soundsphere::StringVec items = soundsphere::explorer_scan_folder(exedir, filters, 1);

    soundsphere::StringVec::iterator it = items.begin();
    for (; it != items.end(); it++)
    {
        std::string& path = *it;
        AddonPtr addon = _new_addon_instance();
        if (ev_dlopen(&addon->lib, path.c_str(), nullptr) != 0)
        {
            continue;
        }

        soundsphere_addon_init fn_init = nullptr;
        if (ev_dlsym(&addon->lib, "soundsphere_addon_init", (void**)&fn_init) != 0)
        {
            continue;
        }

        if (fn_init(&addon->info, &addon->api) != 0)
        {
            continue;
        }
        if (addon->info.id == nullptr || addon->info.exit == nullptr || addon->info.draw == nullptr)
        {
            continue;
        }

        s_addon_ctx->addon_map.insert(AddonMap::value_type(addon->info.id, addon));
    }
}

void soundsphere::addon_exit(void)
{
    delete s_addon_ctx;
    s_addon_ctx = nullptr;
}

void soundsphere::addon_draw(void)
{
    AddonMap::iterator it = s_addon_ctx->addon_map.begin();
    for (; it != s_addon_ctx->addon_map.end(); it++)
    {
        it->second->info.draw();
    }
}

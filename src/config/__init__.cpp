#include <nlohmann/json.hpp>
#include <ev.h>
#include "utils/binary.hpp"
#include "utils/defines.hpp"
#include "utils/env.hpp"
#include "utils/path.hpp"
#include "__init__.hpp"

#define JSON_SERDE_FIELD_TO_JSON(field) json[#field] = type.field;

#define JSON_SERDE_FIELD_FROM_JSON(field)                                                                              \
    if (json.contains(#field))                                                                                         \
    {                                                                                                                  \
        json.at(#field).get_to(type.field);                                                                            \
    }

#define JSON_SERDE(TYPE, ...)                                                                                          \
    void to_json(nlohmann::json &json, const TYPE &type)                                                               \
    {                                                                                                                  \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(JSON_SERDE_FIELD_TO_JSON, __VA_ARGS__))                               \
    }                                                                                                                  \
    void from_json(const nlohmann::json &json, TYPE &type)                                                             \
    {                                                                                                                  \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(JSON_SERDE_FIELD_FROM_JSON, __VA_ARGS__))                             \
    }

typedef struct config_ctx
{
    config_ctx();

    /**
     * @brief Configuration file path.
     */
    std::string path;
} config_ctx_t;

soundsphere::config_t soundsphere::_config;

static config_ctx_t *s_config_ctx = nullptr;

static std::string _get_locale(void)
{
    std::string lang = soundsphere::getenv("LANG");
    if (!lang.empty())
    {
        return soundsphere::string_split(lang, ".")[0];
    }

#if defined(_WIN32)
    LANGID langId = GetUserDefaultUILanguage();
    WCHAR localeName[LOCALE_NAME_MAX_LENGTH];
    if (LCIDToLocaleName(MAKELCID(langId, SORT_DEFAULT), localeName, LOCALE_NAME_MAX_LENGTH, 0) != 0)
    {
        lang = soundsphere::wide_to_utf8(localeName);
        lang = soundsphere::string_replace(lang, "-", "_");
        return lang;
    }
#endif

    return "en_US";
}

namespace soundsphere
{

config_lyric::config_lyric()
{
    auto_center_time_ms = 3 * 1000;
    back_font_color[0] = 1.0f;
    back_font_color[1] = 1.0f;
    back_font_color[2] = 1.0f;
    back_font_color[3] = 1.0f;
    fore_font_color[0] = 1.0f;
    fore_font_color[1] = 0.0f;
    fore_font_color[2] = 0.0f;
    fore_font_color[3] = 1.0f;
}

JSON_SERDE(config_lyric_t, auto_center_time_ms, back_font_color, fore_font_color)

config::config()
{
    language = _get_locale();
    volume = 50;
}

JSON_SERDE(config_t, language, volume, lyric, songs, proxy)

} // namespace soundsphere

config_ctx::config_ctx()
{
    /*
     * Calculate default configuration path.
     */
#if defined(_WIN32)
    std::string app_data = soundsphere::getenv("APPDATA");
    if (!app_data.empty())
    {
        path = app_data + "\\" PROG_ID + "\\config.json";
        return;
    }
#else
    std::string xdg_config_home = soundsphere::getenv("XDG_CONFIG_HOME");
    if (!xdg_config_home.empty())
    {
        path = xdg_config_home + "/" PROJECT_ID "/config.json";
        return;
    }

    std::string home = soundsphere::getenv("HOME");
    if (!home.empty())
    {
        path = home + "/.config/" PROJECT_ID "/config.json";
        return;
    }
#endif

    char buf[4096];
    ev_getcwd(buf, sizeof(buf));
    path = std::string(buf) + "/config.json";
}

static void _config_load(void)
{
    ev_fs_req_t req;
    ssize_t ret = ev_fs_readfile(nullptr, &req, s_config_ctx->path.c_str(), nullptr);
    if (ret < 0)
    {
        return;
    }

    const ev_buf_t *buf = ev_fs_get_filecontent(&req);
    std::string config_data((char *)buf->data, buf->size);
    ev_fs_req_cleanup(&req);

    nlohmann::json json = nlohmann::json::parse(config_data);
    soundsphere::_config = json;
}

void soundsphere::config_init(void)
{
    s_config_ctx = new config_ctx_t;
    _config_load();
}

void soundsphere::config_exit(void)
{
    std::string errinfo;
    config_save(errinfo);

    delete s_config_ctx;
    s_config_ctx = nullptr;
}

bool soundsphere::config_save(std::string &errinfo)
{
    std::string dir = soundsphere::dirname(s_config_ctx->path);
    int ret = ev_fs_mkdir(nullptr, nullptr, dir.c_str(), EV_FS_S_IRWXU, nullptr);
    if (ret != 0)
    {
        errinfo = ev_strerror(ret);
        return false;
    }

    nlohmann::json j = _config;
    std::string config_data = j.dump(2);
    soundsphere::dump(s_config_ctx->path.c_str(), config_data.c_str(), config_data.size());

    return true;
}

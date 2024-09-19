#include <nlohmann/json.hpp>
#include <cstdlib>
#include <ev.h>
#include "utils/defines.hpp"
#include "utils/path.hpp"
#include "__init__.hpp"

typedef struct config_ctx
{
    config_ctx();

    /**
     * @brief Configuration file path.
     */
    std::string path;
} config_ctx_t;

soundsphere::config_t soundsphere::_config;

static config_ctx_t* s_config_ctx = nullptr;

namespace soundsphere {

config::config()
{
    lyric_auto_center_time_ms = 3 * 1000;
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(config, lyric_auto_center_time_ms)

}

config_ctx::config_ctx()
{
    /*
     * Calculate default configuration path.
     */
#if defined(_WIN32)
    const char* app_data = std::getenv("APPDATA");
    if (app_data != nullptr)
    {
        path = std::string(app_data) + "\\" PROG_ID + "\\config.json";
        return;
    }
#else
    const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME");
    if (xdg_config_home != nullptr)
    {
        path = std::string(xdg_config_home) + "/" PROG_ID "/config.json";
        return;
    }

    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        path = std::string(home) + "/.config/" PROG_ID "/config.json";
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
    int ret = ev_fs_readfile(nullptr, &req, s_config_ctx->path.c_str(), nullptr);
    if (ret < 0)
    {
        return;
    }

    const ev_buf_t* buf = ev_fs_get_filecontent(&req);
    std::string config_data((char*)buf->data, buf->size);
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

bool soundsphere::config_save(std::string& errinfo)
{
    std::string dir = soundsphere::dirname(s_config_ctx->path);
    int ret = ev_fs_mkdir(nullptr, nullptr, dir.c_str(), EV_FS_S_IRWXU, nullptr);
    if (ret != 0)
    {
        errinfo = ev_strerror(ret);
        return false;
    }

    nlohmann::json j = _config;
    std::string config_data = j.dump();

    ev_file_t file;
    ret = ev_file_open(nullptr, &file, nullptr, s_config_ctx->path.c_str(),
        EV_FS_O_CREAT | EV_FS_O_WRONLY,
        EV_FS_S_IRUSR | EV_FS_S_IWUSR, nullptr);
    if (ret != 0)
    {
        errinfo = ev_strerror(ret);
        return false;
    }

    ev_file_write(&file, nullptr, config_data.c_str(), config_data.size(), nullptr);
    ev_file_close(&file, nullptr);

    return true;
}

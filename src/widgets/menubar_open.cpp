#include <ev.h>
#include <string>
#include "i18n/__init__.hpp"
#include "runtime/__init__.hpp"
#include "utils/explorer.hpp"
#include "utils/string.hpp"
#include "__init__.hpp"

static const char* s_filters[] = {
    "Music\n*.mp3\n*.wav\n*.aac",
};

static ev_os_thread_t s_open_thread = EV_OS_THREAD_INVALID;

static void _menubar_open_init(void)
{
}

static void _menubar_open_exit(void)
{
    if (s_open_thread != EV_OS_THREAD_INVALID)
    {
        ev_thread_exit(&s_open_thread, EV_INFINITE_TIMEOUT);
        s_open_thread = EV_OS_THREAD_INVALID;
    }
}

static void _handle_files_on_ui(std::shared_ptr<soundsphere::StringVec> paths)
{
    (void)paths;
}

static void _handle_files(const soundsphere::StringVec& paths)
{
    std::shared_ptr<soundsphere::StringVec> obj = std::make_shared<soundsphere::StringVec>(paths);
    soundsphere::runtime_call_in_ui<soundsphere::StringVec>(_handle_files_on_ui, obj);
}

static void _start_open_files_thread(void* arg)
{
    (void)arg;

    soundsphere::StringVec paths;
    if (!soundsphere::explorer_open_files(paths, s_filters, IM_ARRAYSIZE(s_filters)))
    {
        return;
    }

    _handle_files(paths);
}

static void _start_open_folder_thread(void* arg)
{
    (void)arg;

    std::string path;
    if (!soundsphere::explorer_open_folder(path))
    {
        return;
    }

    soundsphere::StringVec paths = soundsphere::explorer_scan_folder(path, s_filters, IM_ARRAYSIZE(s_filters));
    _handle_files(paths);
}

static void _menubar_open_draw(void)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(soundsphere::_i18n->file))
        {
            bool enabled = (s_open_thread == EV_OS_THREAD_INVALID);
            if (ImGui::MenuItem(soundsphere::_i18n->open, nullptr, nullptr, enabled))
            {
                IM_ASSERT(s_open_thread == EV_OS_THREAD_INVALID);
                ev_thread_init(&s_open_thread, NULL, _start_open_files_thread, NULL);
            }
            if (ImGui::MenuItem(soundsphere::_i18n->open_folder, nullptr, nullptr, enabled))
            {
                IM_ASSERT(s_open_thread == EV_OS_THREAD_INVALID);
                ev_thread_init(&s_open_thread, NULL, _start_open_folder_thread, NULL);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (s_open_thread != EV_OS_THREAD_INVALID)
    {
        if (ev_thread_exit(&s_open_thread, 0) == 0)
        {
            s_open_thread = EV_OS_THREAD_INVALID;
        }
    }
}

const soundsphere::widget_t soundsphere::menubar_open = {
_menubar_open_init,
_menubar_open_exit,
_menubar_open_draw,
};

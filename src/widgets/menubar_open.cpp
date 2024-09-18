#include <ev.h>
#include <string>
#include "i18n/__init__.h"
#include "runtime/__init__.hpp"
#include "utils/explorer.hpp"
#include "utils/string.hpp"
#include "__init__.hpp"
#include "ui_filter.hpp"
#include "dummy_player.hpp"

static const char* s_filters[] = {
    "Music\n*.flac\n*.mp3\n*.ogg\n*.voc\n*.wav",
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

static void _handle_files_on_ui(soundsphere::MusicTagPtrVecPtr vec)
{
    /* Update playlist. */
    soundsphere::_G.media_list = vec;
    /* Clear selected item. */
    soundsphere::_G.playlist.selected_id = (uint64_t)-1;

    soundsphere::ui_filter_reset();
    soundsphere::dummy_player_reload();
}

static void _handle_files(const soundsphere::StringVec& paths)
{
    soundsphere::MusicTagPtrVecPtr vec = std::make_shared<soundsphere::MusicTagPtrVec>();

    for (size_t i = 0; i < paths.size(); i++)
    {
        soundsphere::MusicTagPtr obj = std::make_shared<soundsphere::music_tags_t>();
        obj->path = paths[i];

        std::string errinfo;
        if (soundsphere::music_read_tag(*obj, errinfo))
        {
            vec->push_back(obj);
        }
    }

    soundsphere::runtime_call_in_ui<soundsphere::MusicTagPtrVec>(_handle_files_on_ui, vec);
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
        if (ImGui::BeginMenu(soundsphere_i18n->translation->file))
        {
            bool enabled = (s_open_thread == EV_OS_THREAD_INVALID);
            if (ImGui::MenuItem(soundsphere_i18n->translation->open, nullptr,
                nullptr, enabled))
            {
                IM_ASSERT(s_open_thread == EV_OS_THREAD_INVALID);
                ev_thread_init(&s_open_thread, NULL, _start_open_files_thread, NULL);
            }
            if (ImGui::MenuItem(soundsphere_i18n->translation->open_folder,
                nullptr, nullptr, enabled))
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

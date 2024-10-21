#include <ev.h>
#include <string>
#include "i18n/__init__.h"
#include "runtime/__init__.hpp"
#include "utils/binary.hpp"
#include "utils/explorer.hpp"
#include "utils/string.hpp"
#include "__init__.hpp"
#include "ui_filter.hpp"
#include "dummy_player.hpp"

using namespace soundsphere;

typedef struct menubar_open_ctx
{
    menubar_open_ctx();
    virtual ~menubar_open_ctx();

    ev_os_thread_t open_thread;
} menubar_open_ctx_t;

static menubar_open_ctx_t *s_menubar_open_ctx = nullptr;

static const char *s_filters[] = {
    "Music\n*.flac\n*.mp3",
};

menubar_open_ctx::menubar_open_ctx()
{
    open_thread = EV_OS_THREAD_INVALID;
}

menubar_open_ctx::~menubar_open_ctx()
{
    if (open_thread != EV_OS_THREAD_INVALID)
    {
        ev_thread_exit(&open_thread, EV_INFINITE_TIMEOUT);
        open_thread = EV_OS_THREAD_INVALID;
    }
}

static void _menubar_open_init(void)
{
    s_menubar_open_ctx = new menubar_open_ctx_t;
}

static void _menubar_open_exit(void)
{
    delete s_menubar_open_ctx;
    s_menubar_open_ctx = nullptr;
}

static void _handle_open_files_on_ui(soundsphere::MusicTagPtrVecPtr vec)
{
    /* Update playlist. */
    soundsphere::_G.media_list = vec;
    /* Clear selected item. */
    soundsphere::_G.playlist.selected_id = (uint64_t)-1;

    widget_fast_req<UiFilterReset>(WIDGET_ID_UI_FILTER);
    widget_fast_req<DummyPlayerReload>(WIDGET_ID_DUMMY_PLAYER);
}

static void _handle_open_files(const soundsphere::StringVec &paths)
{
    soundsphere::MusicTagPtrVecPtr vec = soundsphere::music_read_tag_v(paths);
    soundsphere::runtime_call_in_ui<soundsphere::MusicTagPtrVec>(_handle_open_files_on_ui, vec);
}

static void _start_open_files_thread(void *arg)
{
    (void)arg;

    soundsphere::StringVec paths;
    if (!soundsphere::explorer_open_files(paths, s_filters, IM_ARRAYSIZE(s_filters)))
    {
        return;
    }

    _handle_open_files(paths);
}

static void _start_open_folder_thread(void *arg)
{
    (void)arg;

    std::string path;
    if (!soundsphere::explorer_open_folder(path))
    {
        return;
    }

    soundsphere::StringVec paths = soundsphere::explorer_scan_folder(path, s_filters, IM_ARRAYSIZE(s_filters));
    _handle_open_files(paths);
}

static void _handle_add_files_on_ui(MusicTagPtrVecPtr vec)
{
    MusicTagPtrVecPtr songs = soundsphere::_G.media_list;
    songs->insert(songs->end(), vec->begin(), vec->end());

    soundsphere::remove_duplicate<MusicTagPtr, uint64_t>(*songs.get(),
                                                         [](const MusicTagPtr &p) { return p->path_hash; });

    widget_fast_req<UiFilterReset>(WIDGET_ID_UI_FILTER);
}

static void _handle_add_files(const StringVec &paths)
{
    MusicTagPtrVecPtr vec = music_read_tag_v(paths);
    runtime_call_in_ui<MusicTagPtrVec>(_handle_add_files_on_ui, vec);
}

static void _start_add_file_thread(void *arg)
{
    (void)arg;
    StringVec paths;
    if (!explorer_open_files(paths, s_filters, IM_ARRAYSIZE(s_filters)))
    {
        return;
    }
    _handle_add_files(paths);
}

static void _start_add_folder_thread(void *arg)
{
    (void)arg;

    std::string path;
    if (!soundsphere::explorer_open_folder(path))
    {
        return;
    }

    soundsphere::StringVec paths = soundsphere::explorer_scan_folder(path, s_filters, IM_ARRAYSIZE(s_filters));
    _handle_add_files(paths);
}

static void _menubar_open_draw(void)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(_T->file))
        {
            bool enabled = (s_menubar_open_ctx->open_thread == EV_OS_THREAD_INVALID);
            if (ImGui::MenuItem(_T->open, nullptr, nullptr, enabled))
            {
                ev_thread_init(&s_menubar_open_ctx->open_thread, NULL, _start_open_files_thread, NULL);
            }
            if (ImGui::MenuItem(_T->open_folder, nullptr, nullptr, enabled))
            {
                ev_thread_init(&s_menubar_open_ctx->open_thread, NULL, _start_open_folder_thread, NULL);
            }
            if (ImGui::MenuItem(_T->add, nullptr, nullptr, enabled))
            {
                ev_thread_init(&s_menubar_open_ctx->open_thread, nullptr, _start_add_file_thread, nullptr);
            }
            if (ImGui::MenuItem(_T->add_folder, nullptr, nullptr, enabled))
            {
                ev_thread_init(&s_menubar_open_ctx->open_thread, nullptr, _start_add_folder_thread, nullptr);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (s_menubar_open_ctx->open_thread != EV_OS_THREAD_INVALID)
    {
        if (ev_thread_exit(&s_menubar_open_ctx->open_thread, 0) == 0)
        {
            s_menubar_open_ctx->open_thread = EV_OS_THREAD_INVALID;
        }
    }
}

const soundsphere::widget_t soundsphere::menubar_open = {
    _menubar_open_init,
    _menubar_open_exit,
    _menubar_open_draw,
    nullptr,
};

#include <imgui.h>
#include "runtime/__init__.hpp"
#include "utils/time.hpp"
#include "dummy_player.hpp"
#include "__init__.hpp"

using namespace soundsphere;

typedef struct statusbar_ctx
{
    statusbar_ctx();

    /**
     * @brief Hash of path.
     */
    uint64_t path_hash;

    /**
     * @brief Music format.
     */
    soundsphere::music_type_t format;

    /**
     * @brief Bitrate in kb/s.
     */
    int bitrate;

    /**
     * @brief Sample rate in Hz.
     */
    int samplerate;

    /**
     * @brief The number of audio channels.
     */
    int channels;

    /**
     * @brief Music duration, in seconds.
     */
    double music_duration;

    /**
     * @brief Event dispatcher.
     */
    Msg::Dispatch evt_dispatcher;
} statusbar_ctx_t;

static statusbar_ctx_t *s_statusbar_ctx = nullptr;

static void _on_evt_play(Msg::Ptr msg)
{
    auto evt = msg->get_evt<DummyPlayerResumeOrPlay>();
    s_statusbar_ctx->music_duration = evt->music_duration;
}

static void _on_evt_stop(Msg::Ptr msg)
{
    (void)msg;
    s_statusbar_ctx->music_duration = 0.0;
}

statusbar_ctx::statusbar_ctx()
{
    evt_dispatcher.set_mode(Msg::TYPE_EVT);
    evt_dispatcher.register_handle<DummyPlayerResumeOrPlay>(_on_evt_play);
    evt_dispatcher.register_handle<DummyPlayerPause>(_on_evt_stop);
}

static void _reset_status(void)
{
    s_statusbar_ctx->path_hash = (uint64_t)-1;
    s_statusbar_ctx->format = soundsphere::MUSIC_NONE;
    s_statusbar_ctx->bitrate = 0;
    s_statusbar_ctx->samplerate = 0;
    s_statusbar_ctx->channels = 0;
    s_statusbar_ctx->music_duration = 0.0;
}

static void _widget_statusbar_init(void)
{
    s_statusbar_ctx = new statusbar_ctx_t;
    _reset_status();
}

static void _widget_statusbar_exit(void)
{
    delete s_statusbar_ctx;
    s_statusbar_ctx = nullptr;
}

static void _widget_statusbar_draw(void)
{
    ImGui::SetNextWindowSize(soundsphere::_layout.statusbar.size);
    ImGui::SetNextWindowPos(soundsphere::_layout.statusbar.pos);

    static char timebuf_pos[16];
    static char timebuf_len[16];

    const int status_bar_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("StatusBar", nullptr, status_bar_flags))
    {
        soundsphere::MusicTagPtr obj = soundsphere::_G.dummy_player.current_music;
        if (obj.get() == nullptr)
        {
            _reset_status();
        }
        else if (obj->path_hash != s_statusbar_ctx->path_hash)
        {
            s_statusbar_ctx->path_hash = obj->path_hash;
            s_statusbar_ctx->format = obj->info.format;
            s_statusbar_ctx->bitrate = obj->info.bitrate;
            s_statusbar_ctx->channels = obj->info.channel;
            s_statusbar_ctx->samplerate = obj->info.samplerate;
        }

        const char *type = soundsphere::music_tag_format_name(s_statusbar_ctx->format);
        soundsphere::time_seconds_to_string(timebuf_pos, sizeof(timebuf_pos), soundsphere::_G.playbar.music_position);
        soundsphere::time_seconds_to_string(timebuf_len, sizeof(timebuf_len), s_statusbar_ctx->music_duration);

        ImGui::Text("%s | %d kbps | %d Hz | %d Channel | %s / %s", type != NULL ? type : "---",
                    s_statusbar_ctx->bitrate, s_statusbar_ctx->samplerate, s_statusbar_ctx->channels, timebuf_pos,
                    timebuf_len);
    }
    ImGui::End();
}

static void _widget_statusbar_message(Msg::Ptr msg)
{
    s_statusbar_ctx->evt_dispatcher.dispatch(msg);
}

const soundsphere::widget_t soundsphere::ui_statusbar = {
    _widget_statusbar_init,
    _widget_statusbar_exit,
    _widget_statusbar_draw,
    _widget_statusbar_message,
};

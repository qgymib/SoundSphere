#include <imgui.h>
#include "runtime/__init__.hpp"
#include "utils/time.hpp"
#include "__init__.hpp"

typedef struct statusbar_ctx
{
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
} statusbar_ctx_t;

static statusbar_ctx_t *s_statusbar_ctx = nullptr;

static void _reset_status(void)
{
    s_statusbar_ctx->path_hash = (uint64_t)-1;
    s_statusbar_ctx->format = soundsphere::MUSIC_NONE;
    s_statusbar_ctx->bitrate = 0;
    s_statusbar_ctx->samplerate = 0;
    s_statusbar_ctx->channels = 0;
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
        soundsphere::time_seconds_to_string(timebuf_len, sizeof(timebuf_len), soundsphere::_G.playbar.music_duration);

        ImGui::Text("%s | %d kbps | %d Hz | %d Channel | %s / %s", type != NULL ? type : "---",
                    s_statusbar_ctx->bitrate, s_statusbar_ctx->samplerate, s_statusbar_ctx->channels, timebuf_pos,
                    timebuf_len);
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_statusbar = {
    _widget_statusbar_init,
    _widget_statusbar_exit,
    _widget_statusbar_draw,
};

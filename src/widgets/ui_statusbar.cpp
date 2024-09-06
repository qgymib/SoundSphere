#include <imgui.h>
#include "runtime/__init__.hpp"
#include "utils/time.hpp"
#include "__init__.hpp"

static void _widget_statusbar_init(void)
{
}

static void _widget_statusbar_exit(void)
{
}

static const char* _music_type_to_str(Mix_MusicType type)
{
    switch (type)
    {
    case MUS_CMD:       return "CMD";
    case MUS_WAV:       return "WAV";
    case MUS_MOD:       return "MOD";
    case MUS_MID:       return "MID";
    case MUS_OGG:       return "OGG";
    case MUS_MP3:       return "MP3";
    case MUS_FLAC:      return "FLAC";
    case MUS_OPUS:      return "OPUS";
    case MUS_WAVPACK:   return "WAVPACK";
    case MUS_GME:       return "GME";
    default:            break;
    }
    return "UNKNOWN";
}

static void _get_properties(int* bitrate, int* samplerate, int* channels)
{
    *bitrate = 0;
    *samplerate = 0;
    *channels = 0;

    if (soundsphere::_G.playlist_play_idx < 0)
    {
        return;
    }

    soundsphere::PlayItem::Ptr obj = soundsphere::_G.playlist->at(soundsphere::_G.playlist_play_idx);

    *bitrate = obj->bitrate;
    *samplerate = obj->samplerate;
    *channels = obj->channels;
}

static void _widget_statusbar_draw(void)
{
    ImGui::SetNextWindowSize(soundsphere::_layout.statusbar.size);
    ImGui::SetNextWindowPos(soundsphere::_layout.statusbar.pos);

    static char timebuf_pos[16];
    static char timebuf_len[16];

    const int status_bar_flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("StatusBar", nullptr, status_bar_flags))
    {
        int bitrate = 0;
        int samplerate = 0;
        int channels = 0;
        _get_properties(&bitrate, &samplerate, &channels);

        const char* type = _music_type_to_str(soundsphere::_G.music_type);
        soundsphere::time_seconds_to_string(timebuf_pos, sizeof(timebuf_pos),
            soundsphere::_G.music_position);
        soundsphere::time_seconds_to_string(timebuf_len, sizeof(timebuf_len),
            soundsphere::_G.music_duration);

        ImGui::Text("%s | %d kbps | %d Hz | %d Channel | %s / %s",
            type, bitrate, samplerate, channels, timebuf_pos, timebuf_len);
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_statusbar = {
_widget_statusbar_init,
_widget_statusbar_exit,
_widget_statusbar_draw,
};

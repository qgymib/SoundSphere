#include <imgui.h>
#include <IconsFontAwesome6.h>
#include <SDL2/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include "runtime/__init__.hpp"
#include "utils/time.hpp"
#include "__init__.hpp"

/**
 * @brief The music we are playing.
 */
static Mix_Music* s_music = nullptr;
static int s_playing_idx = -1;

static void _widget_playbar_init(void)
{
    int ret;

    ret = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048);
    if (ret != 0)
    {
        spdlog::critical("Mix_OpenAudio failed.");
        exit(EXIT_FAILURE);
    }

    Mix_VolumeMusic(soundsphere::_G.volume);
}

static void _stop_play(void)
{
    if (s_music != nullptr)
    {
        Mix_FreeMusic(s_music);
        s_music = nullptr;
    }
    s_playing_idx = -1;

    soundsphere::_G.music_type = MUS_NONE;
    soundsphere::_G.music_position = 0.0;
    soundsphere::_G.music_duration = 0.0;
}

static void _widget_playbar_exit(void)
{
    _stop_play();
    Mix_CloseAudio();
}

static void _widget_playbar_draw(void)
{
    ImGui::SetNextWindowSize(soundsphere::_layout.playbar.size);
    ImGui::SetNextWindowPos(soundsphere::_layout.playbar.pos);

    const int play_bar_flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("PlayBar", nullptr, play_bar_flags))
    {
        if (s_playing_idx != soundsphere::_G.playlist_play_idx && soundsphere::_G.playlist_play_idx >= 0)
        {
            _stop_play();
            s_playing_idx = soundsphere::_G.playlist_play_idx;
            soundsphere::PlayItem::Ptr obj = soundsphere::_G.playlist->at(s_playing_idx);
            spdlog::info("play {}", obj->path);

            s_music = Mix_LoadMUS(obj->path.c_str());
            spdlog::info("music object: {}", (void*)s_music);

            soundsphere::_G.music_type = Mix_GetMusicType(s_music);
            soundsphere::_G.music_duration = Mix_MusicDuration(s_music);
            soundsphere::_G.music_position = 0.0;
            spdlog::info("duration: {}", soundsphere::_G.music_duration);

            int ret = Mix_PlayMusic(s_music, 1);
            spdlog::info("Mix_PlayMusic {}", ret);
        }

        soundsphere::_G.music_position = s_music != nullptr ? Mix_GetMusicPosition(s_music) : 0.0;

        ImGui::Button(ICON_FA_BACKWARD);
        ImGui::SameLine();

        if (!Mix_PausedMusic())
        {
            if (ImGui::Button(ICON_FA_PAUSE))
            {
                Mix_PauseMusic();
            }
        }
        else
        {
            if (ImGui::Button(ICON_FA_PLAY))
            {
                Mix_ResumeMusic();
            }
        }

        ImGui::SameLine();

        ImGui::Button(ICON_FA_FORWARD);
        ImGui::SameLine();

        {
            static char buf[12];
            soundsphere::time_seconds_to_string(buf, sizeof(buf),
                soundsphere::_G.music_position);
            ImGui::Text("%s", buf);
        }
        ImGui::SameLine();

        ImGui::SetNextItemWidth(-256);
        {
            float position_percentage = (soundsphere::_G.music_duration == 0.0) ?
                0.0f : (soundsphere::_G.music_position / soundsphere::_G.music_duration);
            if (ImGui::SliderFloat("##playbar_slider", &position_percentage, 0, 1, "", ImGuiSliderFlags_NoInput))
            {
                double target_position = soundsphere::_G.music_duration * position_percentage;
                Mix_SetMusicPosition(target_position);
            }
            ImGui::SameLine();
        }

        {
            static char buf[12];
            soundsphere::time_seconds_to_string(buf, sizeof(buf),
                soundsphere::_G.music_duration);
            ImGui::Text("%s", buf);
        }
        ImGui::SameLine();

        ImGui::Button(ICON_FA_SHUFFLE);
        ImGui::SameLine();

        ImGui::Text(ICON_FA_VOLUME_HIGH);
        ImGui::SameLine();

        ImGui::SetNextItemWidth(128);
        if (ImGui::SliderInt("##PlayerBarVolume", &soundsphere::_G.volume, 0, MIX_MAX_VOLUME, "", ImGuiSliderFlags_NoInput))
        {
            spdlog::info("volume {}", soundsphere::_G.volume);
            Mix_VolumeMusic(soundsphere::_G.volume);
        }
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_playbar = {
_widget_playbar_init,
_widget_playbar_exit,
_widget_playbar_draw,
};

#include <IconsFontAwesome6.h>
#include <spdlog/spdlog.h>
#include "runtime/__init__.hpp"
#include "utils/time.hpp"
#include "__init__.hpp"
#include "dummy_player.hpp"

static void _widget_playbar_init(void)
{
}

static void _widget_playbar_exit(void)
{
}

static void _widget_playbar_draw_backward_btn(void)
{
    if (ImGui::Button(ICON_FA_BACKWARD))
    {
    }
    ImGui::SameLine();
}

static void _widget_playbar_draw_play_btn(void)
{
    if (soundsphere::_G.playbar.is_playing)
    {
        if (ImGui::Button(ICON_FA_PAUSE))
        {
            soundsphere::dummy_player_pause();
        }
    }
    else
    {
        if (ImGui::Button(ICON_FA_PLAY))
        {
            soundsphere::dummy_player_resume_or_play();
        }
    }
    ImGui::SameLine();
}

static void _widget_playbar_draw_forward_btn(void)
{
    if (ImGui::Button(ICON_FA_FORWARD))
    {
        soundsphere::dummy_player_next();
    }
    ImGui::SameLine();
}

static void _widget_playbar_draw_shuffle_btn(void)
{
    soundsphere::shuffle_mode_t mode = soundsphere::dummy_player_get_shuffle_mode();

    if (mode == soundsphere::SHUFFLE_ORDER)
    {
        if (ImGui::Button(ICON_FA_RIGHT_LEFT))
        {
            soundsphere::dummy_player_set_shuffle(soundsphere::SHUFFLE_RANDOM);
        }
        goto finish;
    }

    if (mode == soundsphere::SHUFFLE_RANDOM)
    {
        if (ImGui::Button(ICON_FA_SHUFFLE))
        {
            soundsphere::dummy_player_set_shuffle(soundsphere::SHUFFLE_REPEAT);
        }
        goto finish;
    }

    if (mode == soundsphere::SHUFFLE_REPEAT)
    {
        if (ImGui::Button(ICON_FA_ROTATE_RIGHT))
        {
            soundsphere::dummy_player_set_shuffle(soundsphere::SHUFFLE_ORDER);
        }
        goto finish;
    }

finish:
    ImGui::SameLine();
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
        _widget_playbar_draw_backward_btn();
        _widget_playbar_draw_play_btn();
        _widget_playbar_draw_forward_btn();

        {
            static char buf[12];
            soundsphere::time_seconds_to_string(buf, sizeof(buf),
                soundsphere::_G.playbar.music_position);
            ImGui::Text("%s", buf);
        }
        ImGui::SameLine();

        ImGui::SetNextItemWidth(-256);
        {
            float position_percentage = (soundsphere::_G.playbar.music_duration == 0.0) ?
                0.0f : (float)(soundsphere::_G.playbar.music_position / soundsphere::_G.playbar.music_duration);
            if (ImGui::SliderFloat("##playbar_slider", &position_percentage, 0, 1, "", ImGuiSliderFlags_NoInput))
            {
                double target_position = soundsphere::_G.playbar.music_duration * position_percentage;
                soundsphere::dummy_player_set_position(target_position);
            }
            ImGui::SameLine();
        }

        {
            static char buf[12];
            soundsphere::time_seconds_to_string(buf, sizeof(buf),
                soundsphere::_G.playbar.music_duration);
            ImGui::Text("%s", buf);
        }
        ImGui::SameLine();

        _widget_playbar_draw_shuffle_btn();

        ImGui::Text(ICON_FA_VOLUME_HIGH);
        ImGui::SameLine();

        ImGui::SetNextItemWidth(128);
        if (ImGui::SliderInt("##PlayerBarVolume", &soundsphere::_G.playbar.volume, 0, MIX_MAX_VOLUME, "", ImGuiSliderFlags_NoInput))
        {
            spdlog::info("volume {}", soundsphere::_G.playbar.volume);
            soundsphere::dummy_player_set_volume(soundsphere::_G.playbar.volume);
        }
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_playbar = {
_widget_playbar_init,
_widget_playbar_exit,
_widget_playbar_draw,
};

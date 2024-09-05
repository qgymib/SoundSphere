#include <imgui.h>
#include <IconsFontAwesome6.h>
#include "runtime/__init__.hpp"
#include "__init__.hpp"

static void _widget_playbar_init(void)
{
}

static void _widget_playbar_exit(void)
{
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
        ImGui::Button(ICON_FA_BACKWARD);
        ImGui::SameLine();

        ImGui::Button(ICON_FA_PLAY);
        ImGui::SameLine();

        ImGui::Button(ICON_FA_FORWARD);
        ImGui::SameLine();

        ImGui::Text("00:00:00");
        ImGui::SameLine();

        static float s = 0;
        ImGui::SliderFloat("##playbar_slider", &s, 0, 1, "", ImGuiSliderFlags_NoInput);
        ImGui::SameLine();

        ImGui::Text("00:00:00");
        ImGui::SameLine();

        ImGui::Button(ICON_FA_SHUFFLE);
        ImGui::SameLine();

        ImGui::Text(ICON_FA_VOLUME_HIGH);
        ImGui::SameLine();

        static float vol = 0;
        ImGui::SliderFloat("##PlayerBarVolume", &vol, 0, 1, "", ImGuiSliderFlags_NoInput);
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_playbar = {
_widget_playbar_init,
_widget_playbar_exit,
_widget_playbar_draw,
};

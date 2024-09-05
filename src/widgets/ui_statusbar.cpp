#include <imgui.h>
#include "runtime/__init__.hpp"
#include "__init__.hpp"

static void _widget_statusbar_init(void)
{
}

static void _widget_statusbar_exit(void)
{
}

static void _widget_statusbar_draw(void)
{
    ImGui::SetNextWindowSize(soundsphere::_layout.statusbar.size);
    ImGui::SetNextWindowPos(soundsphere::_layout.statusbar.pos);

    const int status_bar_flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("StatusBar", nullptr, status_bar_flags))
    {
        ImGui::Text("FLAC | 1468 kbps | 44100 Hz | 立体声 | 1:31 / 5:05");
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_statusbar = {
_widget_statusbar_init,
_widget_statusbar_exit,
_widget_statusbar_draw,
};

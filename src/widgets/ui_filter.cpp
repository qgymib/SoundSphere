#include <imgui.h>
#include "runtime/__init__.hpp"
#include "__init__.hpp"

static void _ui_filter_init(void)
{
}

static void _ui_filter_exit(void)
{
}

static void _ui_filter_draw(void)
{
    ImGui::SetNextWindowSize(soundsphere::_layout.filter.size);
    ImGui::SetNextWindowPos(soundsphere::_layout.filter.pos);

    const int play_list_flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("Filter", nullptr, play_list_flags))
    {
        ImGui::Text("Filter");
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_filter = {
_ui_filter_init,
_ui_filter_exit,
_ui_filter_draw,
};

#include <imgui.h>
#include "runtime/__init__.hpp"
#include "__init__.hpp"

static void _ui_lyric_init(void)
{
}

static void _ui_lyric_exit(void)
{
}

static void _ui_lyric_draw(void)
{
    /* Set position and size. */
    ImGui::SetNextWindowSize(soundsphere::_layout.lyric.size);
    ImGui::SetNextWindowPos(soundsphere::_layout.lyric.pos);

    const int lyric_flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("Lyric", nullptr, lyric_flags))
    {
        ImGui::Text("Lyric");
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_lyric = {
_ui_lyric_init,
_ui_lyric_exit,
_ui_lyric_draw,
};

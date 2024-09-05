#include "__init__.hpp"

static void _ui_cover_init(void)
{
}

static void _ui_cover_exit(void)
{
}

static void _ui_cover_draw(void)
{
    ImGui::SetNextWindowPos(soundsphere::_layout.cover.pos);
    ImGui::SetNextWindowSize(soundsphere::_layout.cover.size);

    const int cover_flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("Cover", nullptr, cover_flags))
    {
        ImGui::Text("Cover");
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_cover = {
_ui_cover_init,
_ui_cover_exit,
_ui_cover_draw,
};

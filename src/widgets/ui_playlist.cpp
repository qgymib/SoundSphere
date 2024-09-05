#include "__init__.hpp"

static void _ui_playlist_init(void)
{
}

static void _ui_playlist_exit(void)
{
}

static void _ui_playlist_draw(void)
{
    ImGui::SetNextWindowPos(soundsphere::_layout.playlist.pos);
    ImGui::SetNextWindowSize(soundsphere::_layout.playlist.size);

    const int cover_flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("PlayList", nullptr, cover_flags))
    {
        ImGui::Text("PlayList");
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_playlist = {
_ui_playlist_init,
_ui_playlist_exit,
_ui_playlist_draw,
};

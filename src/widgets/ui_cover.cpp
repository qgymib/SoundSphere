
#include <SDL_surface.h>
#include <spdlog/spdlog.h>
#include "assets/icon.h"
#include "backends/__init__.hpp"
#include "runtime/__init__.hpp"
#include "__init__.hpp"

static soundsphere::Texture s_default_cover;

static void _ui_cover_init(void)
{
    s_default_cover = soundsphere::backend_load_image(__icon_png, __icon_png_len);
}

static void _ui_cover_exit(void)
{
    s_default_cover.reset();
}

static void _ui_cover_draw(void)
{
    ImGui::SetNextWindowPos(soundsphere::_layout.cover.pos);
    ImGui::SetNextWindowSize(soundsphere::_layout.cover.size);

    const int cover_flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("Cover", nullptr, cover_flags))
    {
        ImVec2 display_sz = soundsphere::_layout.cover.size;
        display_sz.x -= 16;
        display_sz.y -= 16;

        soundsphere::Texture texture = (soundsphere::_G.cover.img.get() != nullptr) ?
            soundsphere::_G.cover.img : s_default_cover;

        ImGui::Image(texture.get(), display_sz);
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_cover = {
_ui_cover_init,
_ui_cover_exit,
_ui_cover_draw,
};

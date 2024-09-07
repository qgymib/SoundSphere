#include <stb_image.h>
#include <SDL_surface.h>
#include <spdlog/spdlog.h>
#include "assets/icon.h"
#include "runtime/__init__.hpp"
#include "__init__.hpp"

static SDL_Texture* s_default_cover = nullptr;
static int img_width = 0;
static int img_height = 0;

static void _ui_cover_init(void)
{
    int channels = 0;
    unsigned char* data = stbi_load_from_memory(__icon_png, __icon_png_len, &img_width, &img_height, &channels, 0);
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(data, img_width, img_height,
        channels * 8, channels * img_width, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    if (surface == nullptr)
    {
        spdlog::error("Failed to create SDL surface: {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    s_default_cover = SDL_CreateTextureFromSurface(soundsphere::_G.renderer, surface);
    SDL_FreeSurface(surface);
    stbi_image_free(data);
}

static void _ui_cover_exit(void)
{
    if (s_default_cover != nullptr)
    {
        SDL_DestroyTexture(s_default_cover);
        s_default_cover = nullptr;
    }
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

        ImGui::Image(s_default_cover, display_sz);
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_cover = {
_ui_cover_init,
_ui_cover_exit,
_ui_cover_draw,
};

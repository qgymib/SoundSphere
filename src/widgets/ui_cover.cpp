#include <SDL_surface.h>
#include <spdlog/spdlog.h>
#include "assets/icon.h"
#include "backends/__init__.hpp"
#include "runtime/__init__.hpp"
#include "__init__.hpp"

typedef struct cover_ctx
{
    cover_ctx();

    /**
     * @brief The default cover.
     */
    soundsphere::Texture default_cover;

    /**
     * @brief The shown cover.
     */
    soundsphere::Texture last_cover;

    /**
     * @brief The id of playing music.
     */
    uint64_t last_show_item_id;
} cover_ctx_t;

static cover_ctx_t *s_cover_ctx = nullptr;

cover_ctx::cover_ctx()
{
    last_show_item_id = (uint64_t)-1;
}

static void _ui_cover_init(void)
{
    s_cover_ctx = new cover_ctx_t;
    s_cover_ctx->default_cover = soundsphere::backend_load_image(__icon_png, __icon_png_len);
}

static void _ui_cover_exit(void)
{
    delete s_cover_ctx;
    s_cover_ctx = nullptr;
}

static void _ui_cover_draw_cover(const ImVec2 &display_sz)
{
    soundsphere::MusicTagPtr music = soundsphere::_G.dummy_player.current_music;
    /* If no music is playing, show default cover. */
    if (music.get() == nullptr)
    {
        ImGui::Image(s_cover_ctx->default_cover.get(), display_sz);
        return;
    }

    /* If current playing music is changed, refresh cover. */
    if (s_cover_ctx->last_show_item_id != music->path_hash)
    {
        s_cover_ctx->last_show_item_id = music->path_hash;

        soundsphere::music_tag_image_t *cover_data = &music->info.covers[0];
        if (cover_data->data.size() != 0)
        {
            s_cover_ctx->last_cover = soundsphere::backend_load_image(cover_data->data.data(), cover_data->data.size());
        }
        else
        {
            s_cover_ctx->last_cover = s_cover_ctx->default_cover;
        }
    }

    ImGui::Image(s_cover_ctx->last_cover.get(), display_sz);
}

static void _ui_cover_draw(void)
{
    ImGui::SetNextWindowPos(soundsphere::_layout.cover.pos);
    ImGui::SetNextWindowSize(soundsphere::_layout.cover.size);

    const int cover_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                            ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("Cover", nullptr, cover_flags))
    {
        ImVec2 display_sz = soundsphere::_layout.cover.size;
        display_sz.x -= 16;
        display_sz.y -= 16;

        _ui_cover_draw_cover(display_sz);
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_cover = {
    _ui_cover_init,
    _ui_cover_exit,
    _ui_cover_draw,
    nullptr,
};

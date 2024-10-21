#include "runtime/__init__.hpp"
#include "__init__.hpp"

typedef struct title_ctx
{
    title_ctx();

    /**
     * @brief Path hash for title.
     */
    uint64_t path_hash;

    /**
     * @brief Music format.
     */
    soundsphere::music_type_t format;

    /**
     * @brief The bitrate in KB/s.
     */
    int bitrate;

    /**
     * @brief Music title.
     */
    std::string title;
} title_ctx_t;

static title_ctx_t *s_title_ctx = nullptr;

title_ctx::title_ctx()
{
    path_hash = (uint64_t)-1;
    bitrate = 0;
    format = soundsphere::MUSIC_NONE;
}

static void _ui_title_init(void)
{
    s_title_ctx = new title_ctx_t;
}

static void _ui_title_exit(void)
{
    delete s_title_ctx;
    s_title_ctx = nullptr;
}

static void _ui_title_draw(void)
{
    ImGui::SetNextWindowPos(soundsphere::_layout.title.pos);
    ImGui::SetNextWindowSize(soundsphere::_layout.title.size);

    const int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                      ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("Title", nullptr, flags))
    {
        soundsphere::MusicTagPtr obj = soundsphere::_G.dummy_player.current_music;
        if (obj.get() == nullptr)
        {
            s_title_ctx->path_hash = (uint64_t)-1;
            s_title_ctx->title.clear();
            s_title_ctx->bitrate = 0;
            s_title_ctx->format = soundsphere::MUSIC_NONE;
        }
        else if (s_title_ctx->path_hash != obj->path_hash)
        {
            s_title_ctx->path_hash = obj->path_hash;
            s_title_ctx->bitrate = obj->info.bitrate;
            s_title_ctx->title = obj->info.title;
            s_title_ctx->format = obj->info.format;
        }

        const char *type = soundsphere::music_tag_format_name(s_title_ctx->format);
        if (type != nullptr)
        {
            ImGui::TextCenter("%s", s_title_ctx->title.c_str());
            ImGui::TextCenter("%s | %d K", type, s_title_ctx->bitrate);
        }
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_title = {
    _ui_title_init,
    _ui_title_exit,
    _ui_title_draw,
    nullptr,
};

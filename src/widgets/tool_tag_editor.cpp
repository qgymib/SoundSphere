#include "i18n/__init__.h"
#include "utils/music_tag.hpp"
#include "__init__.hpp"
#include "tool_tag_editor.hpp"

typedef struct tag_editor_ctx
{
    tag_editor_ctx();

    /**
     * @brief Whether window is opened.
     */
    bool                        window_open;

    /**
     * @brief The default window size.
     */
    ImVec2                      default_window_sz;

    /**
     * @brief Tag information.
     */
    soundsphere::MusicTagPtr    tags;
} tag_editor_ctx_t;

static tag_editor_ctx_t* s_tag_editor = nullptr;

tag_editor_ctx::tag_editor_ctx()
{
    window_open = false;
    default_window_sz = ImVec2(640, 400);
}

static void _tool_tageditor_close(void)
{
    s_tag_editor->tags = std::make_shared<soundsphere::music_tags_t>();
}

static void _tool_tageditor_init(void)
{
    s_tag_editor = new tag_editor_ctx_t;
    _tool_tageditor_close();
}

static void _tool_tageditor_exit(void)
{
    delete s_tag_editor;
    s_tag_editor = nullptr;
}

static void _tool_tageditor_draw_editor(void)
{
    ImGui::InputText(_T->path, &s_tag_editor->tags->path);
    ImGui::InputText(_T->title, &s_tag_editor->tags->info.title);
    ImGui::InputText(_T->artist, &s_tag_editor->tags->info.artist);
    ImGui::InputTextMultiline(_T->lyric, &s_tag_editor->tags->info.lyric);

    if (ImGui::Button(_T->save))
    {
        std::string errinfo;
        soundsphere::music_write_tag(*s_tag_editor->tags, errinfo);
    }
}

static void _tool_tageditor_draw(void)
{
    if (s_tag_editor->window_open)
    {
        //ImGui::SetNextWindowSize(s_tag_editor->default_window_sz, ImGuiCond_FirstUseEver);
        if (ImGui::Begin(_T->tag_editor, &s_tag_editor->window_open, ImGuiWindowFlags_AlwaysAutoResize))
        {
            _tool_tageditor_draw_editor();
        }
        ImGui::End();
    }
}

const soundsphere::widget_t soundsphere::tool_tag_editor = {
_tool_tageditor_init,
_tool_tageditor_exit,
_tool_tageditor_draw,
};

void soundsphere::tag_editor_open(const std::string& path)
{
    _tool_tageditor_close();

    soundsphere::MusicTagPtr tags = std::make_shared<soundsphere::music_tags_t>();
    tags->path = path;

    music_read_tag(*tags);

    s_tag_editor->tags = tags;
    s_tag_editor->window_open = true;
}

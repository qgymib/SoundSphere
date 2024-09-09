#include <imgui.h>
#include <IconsFontAwesome6.h>
#include "i18n/__init__.hpp"
#include "runtime/__init__.hpp"
#include "__init__.hpp"
#include "ui_filter.hpp"

typedef struct ui_filter_ctx
{
    char filter[4096];
} ui_filter_ctx_t;

static ui_filter_ctx_t* s_filter = nullptr;

static void _ui_filter_init(void)
{
    s_filter = new ui_filter_ctx_t;
    s_filter->filter[0] = '\0';
}

static void _ui_filter_exit(void)
{
    delete s_filter;
    s_filter = nullptr;
}

static soundsphere::PlayItem::PtrVecPtr _filter(soundsphere::PlayItem::PtrVecPtr vec, const char* filter)
{
    if (strlen(filter) == 0)
    {
        return vec;
    }

    soundsphere::PlayItem::PtrVecPtr ret = std::make_shared<soundsphere::PlayItem::PtrVec>();
    soundsphere::PlayItem::PtrVec::iterator it = vec->begin();

    for (; it != vec->end(); it++)
    {
        soundsphere::PlayItem::Ptr obj = *it;
        if (obj->title.find(filter) != std::string::npos)
        {
            ret->push_back(obj);
        }
    }

    return ret;
}

static void _do_filter(void)
{
    soundsphere::_G.playlist.show_vec = _filter(soundsphere::_G.media_list, s_filter->filter);
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
        ImGui::SetNextItemWidth(soundsphere::_layout.filter.size.x - 50);
        if (ImGui::InputTextWithHint("##filter_search", soundsphere::_i18n->search_in_playlist, s_filter->filter, sizeof(s_filter->filter)))
        {
            _do_filter();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_XMARK))
        {
            soundsphere::ui_filter_reset();
        }
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_filter = {
_ui_filter_init,
_ui_filter_exit,
_ui_filter_draw,
};

void soundsphere::ui_filter_reset(void)
{
    s_filter->filter[0] = '\0';
    _do_filter();
}

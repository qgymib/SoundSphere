#include <imgui.h>
#include <IconsFontAwesome6.h>
#include <algorithm>
#include <iterator>
#include "i18n/__init__.h"
#include "runtime/__init__.hpp"
#include "ui_filter.hpp"

using namespace soundsphere;

typedef struct ui_filter_ctx
{
    ui_filter_ctx();

    /**
     * @brief Filter buffer.
     */
    char filter[4096];

    /**
     * @brief Search title.
     */
    bool search_title;

    /**
     * @brief Search in artist.
     */
    bool search_artist;

    Msg::Dispatch req_dispatcher;
} ui_filter_ctx_t;

static ui_filter_ctx_t *s_filter = nullptr;
const uint64_t          UiFilterReset::ID;

static std::string toLower(const std::string &str)
{
    std::string lowerStr;
    std::transform(str.begin(), str.end(), std::back_inserter(lowerStr),
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

static bool _search_string(const std::string &str, const std::string &pat)
{
    // Convert both strings to lowercase
    std::string lower_pat = toLower(pat);
    std::string lower_str = toLower(str);

    // Search for the lowercase s1 in lowercase s2
    return lower_str.find(lower_pat) != std::string::npos;
}

static MusicTagPtrVecPtr _filter(MusicTagPtrVecPtr vec, const std::string &filter)
{
    if (filter.empty())
    {
        return vec;
    }

    MusicTagPtrVecPtr        ret = std::make_shared<MusicTagPtrVec>();
    MusicTagPtrVec::iterator it = vec->begin();

    for (; it != vec->end(); it++)
    {
        MusicTagPtr obj = *it;

        if (s_filter->search_title && _search_string(obj->info.title, filter))
        {
            ret->push_back(obj);
        }

        if (s_filter->search_artist && _search_string(obj->info.artist, filter))
        {
            ret->push_back(obj);
        }
    }

    return ret;
}

static void _do_filter(void)
{
    std::string filter = s_filter->filter;
    soundsphere::_G.playlist.show_vec = _filter(soundsphere::_G.media_list, filter);
}

/**
 * @brief Reset filter.
 */
static void _ui_filter_reset(void)
{
    s_filter->filter[0] = '\0';
    _do_filter();
}

static void _on_ui_filter_reset_req(Msg::Ptr msg)
{
    _ui_filter_reset();
    widget_fast_rsp<UiFilterReset>(msg);
}

ui_filter_ctx::ui_filter_ctx()
{
    filter[0] = '\0';
    search_title = true;
    search_artist = true;

    req_dispatcher.set_mode(Msg::TYPE_REQ);
    req_dispatcher.register_handle<UiFilterReset>(_on_ui_filter_reset_req);
}

static void _ui_filter_init(void)
{
    s_filter = new ui_filter_ctx_t;
    _ui_filter_reset();
}

static void _ui_filter_exit(void)
{
    delete s_filter;
    s_filter = nullptr;
}

static void _ui_filter_draw(void)
{
    ImGui::SetNextWindowSize(soundsphere::_layout.filter.size);
    ImGui::SetNextWindowPos(soundsphere::_layout.filter.pos);

    const int play_list_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("Filter", nullptr, play_list_flags))
    {
        ImGui::SetNextItemWidth(soundsphere::_layout.filter.size.x - 50);
        if (ImGui::InputTextWithHint("##filter_search", _T->search_playlist, s_filter->filter,
                                     sizeof(s_filter->filter)))
        {
            _do_filter();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_XMARK))
        {
            _ui_filter_reset();
        }

        if (ImGui::Checkbox(_T->search_title, &s_filter->search_title))
        {
            _do_filter();
        }
        if (ImGui::Checkbox(_T->search_artist, &s_filter->search_artist))
        {
            _do_filter();
        }
    }
    ImGui::End();
}

static void _ui_filter_message(Msg::Ptr msg)
{
    s_filter->req_dispatcher.dispatch(msg);
}

const soundsphere::widget_t soundsphere::ui_filter = {
    _ui_filter_init,
    _ui_filter_exit,
    _ui_filter_draw,
    _ui_filter_message,
};

#include <ev.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "config/__init__.hpp"
#include "i18n/__init__.h"
#include "runtime/__init__.hpp"
#include "utils/curl.hpp"
#include "utils/krc.hpp"
#include "utils/music_tag.hpp"
#include "utils/time.hpp"
#include "__init__.hpp"
#include "tool_tag_editor.hpp"

using namespace soundsphere;

typedef struct lyric_search_item
{
    std::string id;
    std::string title;
    std::string artist;
    std::string accesskey;
    std::string duration;
    std::string lyric;
} lyric_search_item_t;

typedef std::shared_ptr<lyric_search_item_t> LyricSearchPtr;
typedef std::vector<lyric_search_item_t>     LyricSearchItemVec;
typedef std::shared_ptr<LyricSearchItemVec>  LyricSearchItemVecPtr;

typedef struct tag_editor_ctx
{
    tag_editor_ctx();
    ~tag_editor_ctx();

    /**
     * @brief Whether window is opened.
     */
    bool window_open;

    /**
     * @brief The default window size.
     */
    ImVec2 default_window_sz;

    /**
     * @brief Tag information.
     */
    MusicTagPtr tags;

    /**
     * @brief Working thread.
     */
    ev_os_thread_t thread;

    /**
     * @brief The search result of lyric.
     */
    LyricSearchItemVecPtr lyric_search_ret;

    int selected_row;

    Msg::Dispatch req_dispatcher;
} tag_editor_ctx_t;

static tag_editor_ctx_t *s_tag_editor = nullptr;
const uint64_t           TagEditorOpen::ID;

TagEditorOpen::Req::Req(const std::string &path)
{
    this->path = path;
}

static void _tool_tageditor_close(void)
{
    s_tag_editor->tags = std::make_shared<music_tags_t>();
}

/**
 * @brief Open the tag editor with \p path.
 */
static void _tag_editor_open(const std::string &path)
{
    _tool_tageditor_close();

    MusicTagPtr tags = std::make_shared<music_tags_t>();
    tags->path = path;

    music_read_tag(*tags);

    s_tag_editor->tags = tags;
    s_tag_editor->window_open = true;
    s_tag_editor->selected_row = -1;
}

static void _on_open_music_file(Msg::Ptr msg)
{
    auto req = msg->get_req<TagEditorOpen>();
    _tag_editor_open(req->path);
}

tag_editor_ctx::tag_editor_ctx()
{
    window_open = false;
    default_window_sz = ImVec2(640, 400);
    thread = EV_OS_THREAD_INVALID;
    selected_row = -1;

    req_dispatcher.set_mode(Msg::TYPE_REQ);
    req_dispatcher.register_handle<TagEditorOpen>(_on_open_music_file);
}

tag_editor_ctx::~tag_editor_ctx()
{
    if (thread != EV_OS_THREAD_INVALID)
    {
        ev_thread_exit(&thread, EV_INFINITE_TIMEOUT);
        thread = EV_OS_THREAD_INVALID;
    }
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

static void _update_lyric_search_result_ui(LyricSearchItemVecPtr vec)
{
    s_tag_editor->lyric_search_ret = vec;
}

static void _fill_lyric_result_ui(LyricSearchPtr item)
{
    LyricSearchItemVecPtr vec = s_tag_editor->lyric_search_ret;
    for (auto it = vec->begin(); it != vec->end(); it++)
    {
        if (it->id == item->id)
        {
            it->lyric = item->lyric;
            s_tag_editor->tags->info.lyric = item->lyric;
            return;
        }
    }
}

static void _lyric_search_thread(void *arg)
{
    (void)arg;

    EasyCurl::Ptr curl = std::make_shared<EasyCurl>();

    std::string artist = curl->escape(s_tag_editor->tags->info.artist);
    std::string title = curl->escape(s_tag_editor->tags->info.title);
    std::string url =
        "https://lyrics.kugou.com/search?ver=1&man=yes&client=pc&keyword=" + artist + "-" + title + "&hash=";
    curl_easy_setopt(curl->get(), CURLOPT_URL, url.c_str());

    std::string body;
    CURLcode    ret = curl->perform(body);
    if (ret != CURLE_OK)
    {
        const char *errinfo = curl_easy_strerror(ret);
        spdlog::error("curl_easy_perform() failed: {}", errinfo);
        return;
    }

    LyricSearchItemVecPtr vec = std::make_shared<LyricSearchItemVec>();

    nlohmann::json  rsp = nlohmann::json::parse(body);
    nlohmann::json &candidates = rsp["candidates"];
    for (auto it = candidates.begin(); it != candidates.end(); it++)
    {
        lyric_search_item_t item;
        item.id = (*it)["id"].get<std::string>();
        item.accesskey = (*it)["accesskey"].get<std::string>();
        item.artist = (*it)["singer"].get<std::string>();
        item.title = (*it)["song"].get<std::string>();
        double duration = (*it)["duration"].get<double>() / 1000;
        item.duration = time_seconds_to_string(duration);
        vec->push_back(item);
    }

    runtime_call_in_ui<LyricSearchItemVec>(_update_lyric_search_result_ui, vec);
}

static void _lyric_download_thread(void *arg)
{
    lyric_search_item_t *info = static_cast<lyric_search_item_t *>(arg);
    LyricSearchPtr       item = std::make_shared<lyric_search_item_t>(*info);
    delete info;

    EasyCurl::Ptr curl = std::make_shared<EasyCurl>();
    std::string   url = "https://lyrics.kugou.com/download?ver=1&client=pc&id=" + item->id +
                      "&accesskey=" + item->accesskey + "&fmt=krc&charset=utf8";
    curl_easy_setopt(curl->get(), CURLOPT_URL, url.c_str());

    std::string body;
    CURLcode    ret = curl->perform(body);
    if (ret != CURLE_OK)
    {
        return;
    }

    nlohmann::json rsp = nlohmann::json::parse(body);
    std::string    lyric = rsp["content"];
    item->lyric = krc_to_lyric(lyric);

    runtime_call_in_ui<lyric_search_item_t>(_fill_lyric_result_ui, item);
}

static void _download_lyric(const lyric_search_item_t &info)
{
    if (s_tag_editor->thread != EV_OS_THREAD_INVALID)
    {
        return;
    }

    lyric_search_item_t *new_item = new lyric_search_item_t;
    *new_item = info;

    int ret = ev_thread_init(&s_tag_editor->thread, nullptr, _lyric_download_thread, new_item);
    if (ret != 0)
    {
        delete new_item;
    }
}

static void _show_lyric_table(const LyricSearchItemVec *lyrics)
{
    if (ImGui::BeginTable("TagEditorLyricSearchResultTable", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn(_T->title);
        ImGui::TableSetupColumn(_T->artist);
        ImGui::TableSetupColumn(_T->duration);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int)lyrics->size());
        while (clipper.Step())
        {
            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
            {
                const lyric_search_item_t &item = lyrics->at(row_n);
                ImGui::PushID(item.id.c_str());
                ImGui::TableNextRow();
                {
                    ImGui::TableSetColumnIndex(0);
                    bool selected = row_n == s_tag_editor->selected_row;
                    if (ImGui::Selectable(item.title.c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        s_tag_editor->selected_row = row_n;
                    }
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        _download_lyric(item);
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", item.artist.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", item.duration.c_str());
                }
                ImGui::PopID();
            }
        }

        ImGui::EndTable();
    }
}

static void _tool_tageditor_draw_editor(void)
{
    ImGui::BeginGroup();

    ImGui::PushItemWidth(600);
    {
        ImGui::BeginDisabled();
        ImGui::InputText(_T->path, &s_tag_editor->tags->path);
        ImGui::EndDisabled();

        ImGui::Separator();
        ImGui::Text("%s: %d", _T->bit_rate, s_tag_editor->tags->info.bitrate);
        ImGui::Text("%s: %d", _T->sample_rate, s_tag_editor->tags->info.samplerate);
        ImGui::Text("%s: %d", _T->channel, s_tag_editor->tags->info.channel);

        char buff[12];
        soundsphere::time_seconds_to_string(buff, sizeof(buff), s_tag_editor->tags->info.duration);
        ImGui::Text("%s: %s", _T->duration, buff);

        ImGui::Separator();

        ImGui::InputText(_T->title, &s_tag_editor->tags->info.title);
        ImGui::InputText(_T->artist, &s_tag_editor->tags->info.artist);
        ImGui::InputTextMultiline(_T->lyric, &s_tag_editor->tags->info.lyric);
    }
    ImGui::PopItemWidth();

    bool disabled = s_tag_editor->thread != EV_OS_THREAD_INVALID;
    if (disabled)
    {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button(_T->search_lyric))
    {
        ev_thread_init(&s_tag_editor->thread, nullptr, _lyric_search_thread, nullptr);
    }
    if (disabled)
    {
        ImGui::EndDisabled();
    }
    if (s_tag_editor->thread != EV_OS_THREAD_INVALID)
    {
        if (ev_thread_exit(&s_tag_editor->thread, 0) == 0)
        {
            s_tag_editor->thread = EV_OS_THREAD_INVALID;
        }
    }

    ImGui::SameLine();
    if (ImGui::Button(_T->save))
    {
        std::string errinfo;
        music_write_tag(*s_tag_editor->tags, errinfo);
    }

    ImGui::EndGroup();

    LyricSearchItemVecPtr lyrics = s_tag_editor->lyric_search_ret;
    if (lyrics.get() == nullptr)
    {
        return;
    }

    ImGui::SameLine();

    ImVec2 child_sz = ImVec2(320, ImGui::GetTextLineHeightWithSpacing() * 16);
    if (ImGui::BeginChild("TagEditorLyricChildFrame", child_sz, ImGuiChildFlags_FrameStyle))
    {
        _show_lyric_table(lyrics.get());
    }
    ImGui::EndChild();
}

static void _tool_tageditor_draw(void)
{
    if (s_tag_editor->window_open)
    {
        // ImGui::SetNextWindowSize(s_tag_editor->default_window_sz, ImGuiCond_FirstUseEver);
        if (ImGui::Begin(_T->tag_editor, &s_tag_editor->window_open, ImGuiWindowFlags_AlwaysAutoResize))
        {
            _tool_tageditor_draw_editor();
        }
        ImGui::End();
    }
}

static void _tool_tageditor_message(Msg::Ptr msg)
{
    s_tag_editor->req_dispatcher.dispatch(msg);
}

const soundsphere::widget_t soundsphere::tool_tag_editor = {
    _tool_tageditor_init,
    _tool_tageditor_exit,
    _tool_tageditor_draw,
    _tool_tageditor_message,
};

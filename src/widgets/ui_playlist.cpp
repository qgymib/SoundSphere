#include <spdlog/spdlog.h>
#include "i18n/__init__.hpp"
#include "runtime/__init__.hpp"
#include "__init__.hpp"
#include "dummy_player.hpp"

static void _ui_playlist_init(void)
{
}

static void _ui_playlist_exit(void)
{
}

static void _ui_playlist_draw_table_item(soundsphere::PlayItem* obj)
{
    ImGui::TableSetColumnIndex(0);

    ImGui::BeginGroup();
    {
        static ImVec2 img_sz(64, 64);
        ImGui::Image(obj->cover_texture.get(), img_sz);
        ImGui::SameLine();

        bool is_playing = soundsphere::_G.dummy_player.playing_id == obj->uid;
        bool is_selected = soundsphere::_G.playlist.selected_id == obj->uid;

        if (is_playing)
        {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGuiCol_TabSelected));
        }
        else if (is_selected)
        {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGuiCol_Header));
        }

        ImGui::BeginGroup();
        {
            ImGui::Text("%s", obj->title.c_str());
            ImGui::Text("%s", obj->artist.c_str());
        }
        ImGui::EndGroup();

        ImGui::Dummy(ImVec2(soundsphere::_layout.playlist.size.x, 0));
    }
    ImGui::EndGroup();

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
    {
        soundsphere::_G.playlist.selected_id = obj->uid;
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        soundsphere::dummy_player_resume_or_play();
    }
}

static void _ui_playlist_draw_table(soundsphere::PlayItem::PtrVec* vec)
{
    ImGuiListClipper clipper;
    clipper.Begin((int)vec->size());

    while (clipper.Step())
    {
        for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
        {
            soundsphere::PlayItem::Ptr obj = vec->at(row_n);
            obj->compile_cover();

            ImGui::PushID(obj->path.c_str());
            ImGui::TableNextRow();
            _ui_playlist_draw_table_item(obj.get());
            ImGui::PopID();
        }
    }
}

static void _ui_playlist_draw_window(void)
{
    soundsphere::PlayItem::PtrVecPtr vec = soundsphere::_G.playlist.show_vec;

    const int table_flags = ImGuiTableFlags_Resizable;
    if (ImGui::BeginTable("PlayListTable", 1, table_flags))
    {
        if (vec.get() != nullptr)
        {
            _ui_playlist_draw_table(vec.get());
        }

        ImGui::EndTable();
    }
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
        _ui_playlist_draw_window();
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_playlist = {
_ui_playlist_init,
_ui_playlist_exit,
_ui_playlist_draw,
};

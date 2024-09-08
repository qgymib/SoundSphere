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

static void _ui_playlist_draw2(void)
{
    soundsphere::PlayItem::PtrVecPtr vec = soundsphere::_G.media_list;

    const int table_flags = ImGuiTableFlags_Resizable;
    const char* table_colums[] = {
        soundsphere::_i18n->title,
        soundsphere::_i18n->artist,
    };

    if (ImGui::BeginTable("PlayListTable", 2, table_flags))
    {
        for (size_t i = 0; i < IM_ARRAYSIZE(table_colums); i++)
        {
            const char* title = table_colums[i];
            ImGui::TableSetupColumn(title);
        }
        ImGui::TableHeadersRow();

        if (vec.get() != nullptr)
        {
            ImGuiListClipper clipper;
            clipper.Begin((int)vec->size());

            while (clipper.Step())
            {
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                {
                    soundsphere::PlayItem::Ptr obj = vec->at(row_n);

                    ImGui::PushID(obj->path.c_str());

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);

                    bool is_selected = soundsphere::_G.playlist.selected_idx == row_n;
                    if (ImGui::Selectable(obj->title.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        soundsphere::_G.playlist.selected_idx = row_n;
                    }
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    {
                        soundsphere::dummy_player_resume_or_play();
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", obj->artist.c_str());

                    ImGui::PopID();
                }
            }
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
        _ui_playlist_draw2();
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_playlist = {
_ui_playlist_init,
_ui_playlist_exit,
_ui_playlist_draw,
};

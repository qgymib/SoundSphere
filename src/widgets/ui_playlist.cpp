#include <ev.h>
#include <map>
#include <spdlog/spdlog.h>
#include "backends/__init__.hpp"
#include "i18n/__init__.h"
#include "runtime/__init__.hpp"
#include "utils/time.hpp"
#include "__init__.hpp"
#include "dummy_player.hpp"
#include "tool_tag_editor.hpp"

using namespace soundsphere;

/**
 * @brief Texture map.
 * The key is uid, the value is the texture.
 */
typedef std::map<uint64_t, soundsphere::Texture> TextureMap;

/**
 * @brief Texture record.
 * This must be a POD-struct.
 */
typedef struct texture_ts_record
{
    ev_map_node_t node_ts;
    ev_map_node_t node_uid;

    uint64_t path_hash; /**< Music uid. */
    uint64_t ts;        /**< Last used timestamp. */

    ImTextureID texture; /* Raw pointer to texture. */
} texture_ts_record_t;

/*
 * Ensure #texture_ts_record_t is POD.
 */
static_assert(std::is_pod<texture_ts_record_t>::value);

typedef struct playlist_ctx
{
    playlist_ctx();
    ~playlist_ctx();

    /*
     * The Texture always store in #texture_table.
     *
     * Due to the C++ map is expensive to insert and erase, we use Intrusive data
     * structures #texture_ts_map and #texture_uid_map to maintain GC.
     */
    TextureMap texture_table;
    ev_map_t   texture_ts_map;
    ev_map_t   texture_uid_map;
} playlist_ctx_t;

static playlist_ctx_t *s_playlist_ctx = nullptr;

static int _texture_ts_map_on_cmp(const ev_map_node_t *key1, const ev_map_node_t *key2, void *arg)
{
    (void)arg;
    texture_ts_record_t *rec_1 = EV_CONTAINER_OF(key1, texture_ts_record_t, node_ts);
    texture_ts_record_t *rec_2 = EV_CONTAINER_OF(key2, texture_ts_record_t, node_ts);

    if (rec_1->ts == rec_2->ts)
    {
        if (rec_1->path_hash == rec_2->path_hash)
        {
            return 0;
        }
        return rec_1->path_hash > rec_2->path_hash ? 1 : -1;
    }
    return rec_1->ts > rec_2->ts ? 1 : -1;
}

static int _texture_uid_map_on_cmp(const ev_map_node_t *key1, const ev_map_node_t *key2, void *arg)
{
    (void)arg;
    texture_ts_record_t *rec_1 = EV_CONTAINER_OF(key1, texture_ts_record_t, node_uid);
    texture_ts_record_t *rec_2 = EV_CONTAINER_OF(key2, texture_ts_record_t, node_uid);

    if (rec_1->path_hash == rec_2->path_hash)
    {
        return 0;
    }
    return rec_1->path_hash > rec_2->path_hash ? 1 : -1;
}

playlist_ctx::playlist_ctx()
{
    ev_map_init(&texture_ts_map, _texture_ts_map_on_cmp, NULL);
    ev_map_init(&texture_uid_map, _texture_uid_map_on_cmp, NULL);
}

playlist_ctx::~playlist_ctx()
{
    ev_map_node_t *it = ev_map_begin(&texture_ts_map);
    while (it != nullptr)
    {
        texture_ts_record_t *rec = EV_CONTAINER_OF(it, texture_ts_record_t, node_ts);
        it = ev_map_next(it);
        ev_map_erase(&texture_ts_map, &rec->node_ts);
        ev_map_erase(&texture_uid_map, &rec->node_uid);

        texture_table.erase(rec->path_hash);
        ev_free(rec);
    }
}

static void _ui_playlist_init(void)
{
    s_playlist_ctx = new playlist_ctx_t;
}

static void _ui_playlist_exit(void)
{
    delete s_playlist_ctx;
    s_playlist_ctx = nullptr;
}

static ImTextureID _ui_playlist_compile_cover(soundsphere::music_tags_t *obj, uint64_t ts)
{
    if (!obj->valid)
    {
        return nullptr;
    }

    texture_ts_record_t tmp_rec;
    tmp_rec.path_hash = obj->path_hash;
    ev_map_node_t *it = ev_map_find(&s_playlist_ctx->texture_uid_map, &tmp_rec.node_uid);

    /* If record exist, update the timestamp. */
    if (it != nullptr)
    {
        texture_ts_record_t *rec = EV_CONTAINER_OF(it, texture_ts_record_t, node_uid);

        /* Erase, update timestamp, and insert back. */
        ev_map_erase(&s_playlist_ctx->texture_ts_map, &rec->node_ts);
        rec->ts = ts;
        ev_map_insert(&s_playlist_ctx->texture_ts_map, &rec->node_ts);

        return rec->texture;
    }

    /* If record not exist, compile the cover. */
    soundsphere::music_tag_image_t *cover_data = &obj->info.covers[0];
    soundsphere::Texture texture = soundsphere::backend_load_image(cover_data->data.data(), cover_data->data.size());
    s_playlist_ctx->texture_table.insert(TextureMap::value_type(obj->path_hash, texture));

    texture_ts_record_t *rec = (texture_ts_record_t *)ev_malloc(sizeof(texture_ts_record_t));
    rec->path_hash = obj->path_hash;
    rec->ts = ts;
    rec->texture = texture.get();

    ev_map_insert(&s_playlist_ctx->texture_ts_map, &rec->node_ts);
    ev_map_insert(&s_playlist_ctx->texture_uid_map, &rec->node_uid);

    return rec->texture;
}

static void _ui_playlist_draw_table_item(soundsphere::music_tags_t *obj, uint64_t ts)
{
    ImTextureID texture = _ui_playlist_compile_cover(obj, ts);

    ImGui::TableSetColumnIndex(0);

    ImGui::BeginGroup();
    {
        static ImVec2 img_sz(64, 64);
        if (texture != nullptr)
        {
            ImGui::Image(texture, img_sz);
        }
        else
        {
            ImGui::Dummy(img_sz);
        }

        ImGui::SameLine();

        bool is_playing = soundsphere::_G.dummy_player.current_music.get() == obj;
        bool is_selected = soundsphere::_G.playlist.selected_id == obj->path_hash;

        if (is_playing)
        {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGuiCol_TabSelected));
        }
        else if (is_selected)
        {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGuiCol_Header));
        }

        ImGui::BeginGroup();
        if (obj->valid)
        {
            ImGui::Text("%s", obj->info.title.c_str());
            ImGui::Text("%s", obj->info.artist.c_str());
        }
        else
        {
            ImVec4 color(1.0f, 0.0f, 0.0f, 1.0f);
            ImGui::TextColored(color, "%s.", obj->path.c_str());
        }
        ImGui::EndGroup();

        ImGui::Dummy(ImVec2(soundsphere::_layout.playlist.size.x, 0));
    }
    ImGui::EndGroup();

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        soundsphere::_G.playlist.selected_id = obj->path_hash;
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        widget_fast_req<DummyPlayerResumeOrPlay>(WIDGET_ID_DUMMY_PLAYER);
    }
    if (ImGui::BeginPopupContextItem("Tag Editor##435bf70d-067a-43ca-96aa-423071a6f817"))
    {
        if (ImGui::MenuItem(_T->tag_editor))
        {
            soundsphere::widget_fast_req<soundsphere::TagEditorOpen>(soundsphere::WIDGET_ID_TOOL_TAG_EDITOR, obj->path);
        }
        ImGui::EndPopup();
    }
}

static void _ui_playlist_draw_table(soundsphere::MusicTagPtrVec *vec)
{
    ImGuiListClipper clipper;
    clipper.Begin((int)vec->size());

    /* Get current time. */
    uint64_t ts = soundsphere::clock_time_ms();

    /* Draw table. */
    while (clipper.Step())
    {
        for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
        {
            soundsphere::MusicTagPtr obj = vec->at(row_n);

            ImGui::PushID(obj->path.c_str());
            ImGui::TableNextRow();
            _ui_playlist_draw_table_item(obj.get(), ts);
            ImGui::PopID();
        }
    }
}

static void _ui_playlist_draw_window(void)
{
    soundsphere::MusicTagPtrVecPtr vec = soundsphere::_G.playlist.show_vec;

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

static void _ui_playlist_gc(void)
{
    const uint64_t ts = soundsphere::clock_time_ms();
    const uint64_t gc_timeout = (uint64_t)10 * 1000 * 1000 * 1000;

    uint64_t       gc_cnt = 0;
    ev_map_node_t *it = ev_map_begin(&s_playlist_ctx->texture_ts_map);
    while (it != NULL)
    {
        texture_ts_record_t *rec = EV_CONTAINER_OF(it, texture_ts_record_t, node_ts);
        it = ev_map_next(it);

        uint64_t diff_ts = ts - rec->ts;
        if (diff_ts < gc_timeout)
        {
            break;
        }

        ev_map_erase(&s_playlist_ctx->texture_ts_map, &rec->node_ts);
        ev_map_erase(&s_playlist_ctx->texture_uid_map, &rec->node_uid);
        s_playlist_ctx->texture_table.erase(rec->path_hash);

        ev_free(rec);
        gc_cnt++;
    }

    if (gc_cnt > 0)
    {
        spdlog::debug("playlist: gc {} texture", gc_cnt);
    }
}

static void _ui_playlist_draw(void)
{
    ImGui::SetNextWindowPos(soundsphere::_layout.playlist.pos);
    ImGui::SetNextWindowSize(soundsphere::_layout.playlist.size);

    const int cover_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("PlayList", nullptr, cover_flags))
    {
        _ui_playlist_draw_window();
        _ui_playlist_gc();
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::ui_playlist = {
    _ui_playlist_init,
    _ui_playlist_exit,
    _ui_playlist_draw,
    nullptr,
};

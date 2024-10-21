#include <IconsFontAwesome6.h>
#include <spdlog/spdlog.h>
#include "config/__init__.hpp"
#include "runtime/__init__.hpp"
#include "utils/time.hpp"
#include "__init__.hpp"
#include "dummy_player.hpp"

using namespace soundsphere;

typedef struct playbar_ctx
{
    playbar_ctx();
    ~playbar_ctx();

    DummyPlayerSetShuffleMode::shuffle_mode shuffle_mode;
    Msg::Dispatch evt_dispatcher;
} playbar_ctx_t;

static playbar_ctx_t *s_playbar_ctx = nullptr;

static void _on_shuffle_mode_event(Msg::Ptr msg)
{
    auto evt = msg->get_evt<DummyPlayerSetShuffleMode>();
    s_playbar_ctx->shuffle_mode = evt->mode;
}

playbar_ctx::playbar_ctx()
{
    shuffle_mode = DummyPlayerSetShuffleMode::SHUFFLE_ORDER;
    evt_dispatcher.set_mode(Msg::TYPE_EVT);
    evt_dispatcher.register_handle<DummyPlayerSetShuffleMode>(_on_shuffle_mode_event);
}

playbar_ctx::~playbar_ctx()
{
}

static void _widget_playbar_init(void)
{
    s_playbar_ctx = new playbar_ctx_t;
}

static void _widget_playbar_exit(void)
{
    delete s_playbar_ctx;
    s_playbar_ctx = nullptr;
}

static void _widget_playbar_draw_backward_btn(void)
{
    if (ImGui::Button(ICON_FA_BACKWARD))
    {
    }
}

static void _widget_playbar_draw_play_btn(void)
{
    if (soundsphere::_G.playbar.is_playing)
    {
        if (ImGui::Button(ICON_FA_PAUSE))
        {
            widget_fast_req<DummyPlayerPause>(WIDGET_ID_DUMMY_PLAYER);
        }
    }
    else
    {
        if (ImGui::Button(ICON_FA_PLAY))
        {
            widget_fast_req<DummyPlayerResumeOrPlay>(WIDGET_ID_DUMMY_PLAYER);
        }
    }
}

static void _widget_playbar_draw_forward_btn(void)
{
    if (ImGui::Button(ICON_FA_FORWARD))
    {
        widget_fast_req<DummyPlayerNext>(WIDGET_ID_DUMMY_PLAYER);
    }
}

static void _widget_playbar_draw_shuffle_btn(void)
{
    DummyPlayerSetShuffleMode::shuffle_mode mode = s_playbar_ctx->shuffle_mode;

    if (mode == DummyPlayerSetShuffleMode::SHUFFLE_ORDER)
    {
        if (ImGui::Button(ICON_FA_RIGHT_LEFT))
        {
            widget_fast_req<DummyPlayerSetShuffleMode>(WIDGET_ID_DUMMY_PLAYER,
                                                       DummyPlayerSetShuffleMode::SHUFFLE_RANDOM);
        }
        return;
    }

    if (mode == DummyPlayerSetShuffleMode::SHUFFLE_RANDOM)
    {
        if (ImGui::Button(ICON_FA_SHUFFLE))
        {
            widget_fast_req<DummyPlayerSetShuffleMode>(WIDGET_ID_DUMMY_PLAYER,
                                                       DummyPlayerSetShuffleMode::SHUFFLE_REPEAT);
        }
        return;
    }

    if (mode == DummyPlayerSetShuffleMode::SHUFFLE_REPEAT)
    {
        if (ImGui::Button(ICON_FA_ROTATE_RIGHT))
        {
            widget_fast_req<DummyPlayerSetShuffleMode>(WIDGET_ID_DUMMY_PLAYER,
                                                       DummyPlayerSetShuffleMode::SHUFFLE_ORDER);
        }
        return;
    }
}

static void _widget_playbar_draw_processbar(void)
{
    static uint64_t last_click_time = 0;

    float position_percentage =
        (soundsphere::_G.playbar.music_duration == 0.0)
            ? 0.0f
            : (float)(soundsphere::_G.playbar.music_position / soundsphere::_G.playbar.music_duration);
    if (!ImGui::SliderFloat("##playbar_slider", &position_percentage, 0, 1, "", ImGuiSliderFlags_NoInput))
    {
        return;
    }

    /*
     * Use jitter timeout to avoid user long press process bar.
     *
     * If user long press process bar, this function is called in FPS frequency,
     * AKA 30 / 60 / 120hz.
     *
     * If the FPS is 30, it means the interval between function calls is approximately
     * 34 milliseconds, so the jitter timeout is 34 milliseconds.
     *
     * The lower FPS, the higher the jitter value is. Here 100ms should be enough.
     */
    uint64_t now_time = soundsphere::clock_time_ms();
    if (now_time - last_click_time > 100)
    {
        widget_fast_req<DummyPlayerSetPosition>(WIDGET_ID_DUMMY_PLAYER, position_percentage);
    }

    last_click_time = now_time;
}

static void _widget_playbar_draw(void)
{
    ImGui::SetNextWindowSize(soundsphere::_layout.playbar.size);
    ImGui::SetNextWindowPos(soundsphere::_layout.playbar.pos);

    const int play_bar_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                               ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("PlayBar", nullptr, play_bar_flags))
    {
        _widget_playbar_draw_backward_btn();
        ImGui::SameLine();
        _widget_playbar_draw_play_btn();
        ImGui::SameLine();
        _widget_playbar_draw_forward_btn();
        ImGui::SameLine();

        {
            static char buf[12];
            soundsphere::time_seconds_to_string(buf, sizeof(buf), soundsphere::_G.playbar.music_position);
            ImGui::Text("%s", buf);
        }
        ImGui::SameLine();

        _widget_playbar_draw_processbar();
        ImGui::SameLine();

        {
            static char buf[12];
            soundsphere::time_seconds_to_string(buf, sizeof(buf), soundsphere::_G.playbar.music_duration);
            ImGui::Text("%s", buf);
        }
        ImGui::SameLine();

        _widget_playbar_draw_shuffle_btn();
        ImGui::SameLine();

        ImGui::Text(ICON_FA_VOLUME_HIGH);
        ImGui::SameLine();

        ImGui::SetNextItemWidth(-1);
        int old_volume = soundsphere::_config.volume;
        if (ImGui::SliderInt("##PlayerBarVolume", &old_volume, 0, 100, "", ImGuiSliderFlags_NoInput))
        {
            if (old_volume != soundsphere::_config.volume)
            {
                soundsphere::_config.volume = old_volume;
                widget_fast_req<DummyPlayerSetVolume>(WIDGET_ID_DUMMY_PLAYER, soundsphere::_config.volume);
            }
        }
    }
    ImGui::End();
}

static void _widget_playbar_message(Msg::Ptr msg)
{
    s_playbar_ctx->evt_dispatcher.dispatch(msg);
}

const soundsphere::widget_t soundsphere::ui_playbar = {
    _widget_playbar_init,
    _widget_playbar_exit,
    _widget_playbar_draw,
    _widget_playbar_message,
};

#include <ev.h>
#include <random>
#include <algorithm>
#include <SDL_mixer.h>
#include <spdlog/spdlog.h>
#include "config/__init__.hpp"
#include "runtime/__init__.hpp"
#include "utils/time.hpp"
#include "dummy_player.hpp"
#include "__init__.hpp"

using namespace soundsphere;

typedef struct dummy_player
{
    dummy_player();

    /**
     * @brief SDL music instance.
     */
    Mix_Music *music_mix;

    /**
     * @brief Musics in play order.
     */
    MusicTagPtrVecPtr shuffle_vec;

    /**
     * @brief Is in shuffle mode.
     */
    DummyPlayerSetShuffleMode::shuffle_mode shuffle_mode;

    Msg::Dispatch req_dispatcher;
} dummy_player_t;

static dummy_player_t *s_player = nullptr;
const uint64_t         DummyPlayerReload::ID;
const uint64_t         DummyPlayerPause::ID;
const uint64_t         DummyPlayerNext::ID;
const uint64_t         DummyPlayerSetVolume::ID;
const uint64_t         DummyPlayerSetPosition::ID;
const uint64_t         DummyPlayerSetShuffleMode::ID;
const uint64_t         DummyPlayerResumeOrPlay::ID;

DummyPlayerSetVolume::Req::Req(int volume)
{
    this->volume = volume;
}

DummyPlayerSetPosition::Req::Req(float position)
{
    this->position = position;
}

DummyPlayerSetShuffleMode::Req::Req(shuffle_mode mode)
{
    this->mode = mode;
}

DummyPlayerSetShuffleMode::Evt::Evt(shuffle_mode mode)
{
    this->mode = mode;
}

/**
 * @brief Set current playing position.
 */
static void _dummy_player_set_position(float position)
{
    double real_position = soundsphere::_G.playbar.music_duration * position;
    Mix_SetMusicPosition(real_position);
}

static void _stop_play(void)
{
    if (s_player->music_mix != NULL)
    {
        Mix_FreeMusic(s_player->music_mix);
        s_player->music_mix = NULL;
    }

    soundsphere::_G.playbar.is_playing = false;
    soundsphere::_G.playbar.music_position = 0.0;
    soundsphere::_G.playbar.music_duration = 0.0;

    _dummy_player_set_position(0);
}

static MusicTagPtrVecPtr _shuffle_media(MusicTagPtrVecPtr vec)
{
    MusicTagPtrVecPtr ret = std::make_shared<MusicTagPtrVec>(*vec);

    unsigned seed = (unsigned)(clock_time_ms() / 1000);
    std::shuffle(ret->begin(), ret->end(), std::default_random_engine(seed));

    return ret;
}

static void _dummy_player_reshuffle(void)
{
    switch (s_player->shuffle_mode)
    {
    case DummyPlayerSetShuffleMode::SHUFFLE_ORDER:
        s_player->shuffle_vec = soundsphere::_G.media_list;
        break;
    case DummyPlayerSetShuffleMode::SHUFFLE_REPEAT:
        s_player->shuffle_vec->clear();
        s_player->shuffle_vec->push_back(soundsphere::_G.dummy_player.current_music);
        break;
    case DummyPlayerSetShuffleMode::SHUFFLE_RANDOM:
        s_player->shuffle_vec = _shuffle_media(soundsphere::_G.media_list);
        break;
    }
}

static void _soundsphere_dummy_player_reload(void)
{
    _stop_play();
    _dummy_player_reshuffle();
}

static void _on_reload_req(soundsphere::Msg::Ptr msg)
{
    _soundsphere_dummy_player_reload();
    widget_fast_rsp<DummyPlayerReload>(msg);
}

/**
 * @brief Pause the audio.
 */
static void _soundsphere_dummy_player_pause(void)
{
    Mix_PauseMusic();
    soundsphere::_G.playbar.is_playing = false;
}

static void _on_pause_req(soundsphere::Msg::Ptr msg)
{
    _soundsphere_dummy_player_pause();
    widget_fast_rsp<DummyPlayerPause>(msg);
}

static void _play(soundsphere::MusicTagPtr obj)
{
    soundsphere::_G.dummy_player.current_music = obj;

    s_player->music_mix = Mix_LoadMUS(obj->path.c_str());
    Mix_PlayMusic(s_player->music_mix, 1);

    soundsphere::_G.playbar.is_playing = true;
    soundsphere::_G.playbar.music_duration = Mix_MusicDuration(s_player->music_mix);
    soundsphere::_G.playbar.music_position = 0.0;
}

/**
 * @brief Next music.
 */
static void _soundsphere_dummy_player_next(void)
{
    size_t            idx = 0;
    MusicTagPtrVecPtr vec = s_player->shuffle_vec;

    _stop_play();

    if (vec->empty())
    {
        return;
    }

    for (; idx < vec->size(); idx++)
    {
        MusicTagPtr obj = vec->at(idx);
        if (obj.get() == soundsphere::_G.dummy_player.current_music.get())
        {
            break;
        }
    }

    if (idx >= vec->size())
    {
        idx = 0;
    }
    else
    {
        idx++;
        if (idx >= vec->size())
        {
            idx = 0;
        }
    }

    MusicTagPtr obj = vec->at(idx);
    _play(obj);
}

static void _on_next_req(Msg::Ptr msg)
{
    _soundsphere_dummy_player_next();
    widget_fast_rsp<DummyPlayerNext>(msg);
}

/**
 * @brief Set volume.
 */
static void _soundsphere_dummy_player_set_volume(int volume)
{
    int real_volume = (int)(((float)volume / 100.0f) * MIX_MAX_VOLUME);
    Mix_VolumeMusic(real_volume);
}

static void _on_set_volume_req(Msg::Ptr msg)
{
    auto req = msg->get_req<DummyPlayerSetVolume>();
    _soundsphere_dummy_player_set_volume(req->volume);
    widget_fast_rsp<DummyPlayerSetVolume>(msg);
}

static void _on_set_position(Msg::Ptr msg)
{
    auto req = msg->get_req<DummyPlayerSetPosition>();
    _dummy_player_set_position(req->position);
    widget_fast_rsp<DummyPlayerSetPosition>(msg);
}

/**
 * @brief Shuffle musics.
 */
static void _dummy_player_set_shuffle(DummyPlayerSetShuffleMode::shuffle_mode mode)
{
    s_player->shuffle_mode = mode;
    _dummy_player_reshuffle();
}

static void _on_set_shuffle_mode(Msg::Ptr msg)
{
    auto req = msg->get_req<DummyPlayerSetShuffleMode>();
    _dummy_player_set_shuffle(req->mode);
    widget_fast_rsp<DummyPlayerSetShuffleMode>(msg);
    widget_fast_evt<DummyPlayerSetShuffleMode>(req->mode);
}

static MusicTagPtr _find_audio(uint64_t id)
{
    MusicTagPtrVecPtr vec = soundsphere::_G.media_list;

    for (size_t i = 0; i < vec->size(); i++)
    {
        MusicTagPtr obj = vec->at(i);
        if (obj->path_hash == id)
        {
            return obj;
        }
    }

    return MusicTagPtr();
}

static void _dummy_player_resume_or_play(void)
{
    /* If we have music paused, we resume the music. */
    if (s_player->music_mix != NULL)
    {
        if (Mix_PausedMusic())
        {
            Mix_ResumeMusic();
            soundsphere::_G.playbar.is_playing = true;
            return;
        }
    }

    _stop_play();

    /* Find the song need to play. s*/
    MusicTagPtr obj = _find_audio(soundsphere::_G.playlist.selected_id);
    if (obj.get() == nullptr)
    {
        return;
    }

    _play(obj);

    if (s_player->shuffle_mode == DummyPlayerSetShuffleMode::SHUFFLE_REPEAT)
    {
        _dummy_player_set_shuffle(DummyPlayerSetShuffleMode::SHUFFLE_REPEAT);
    }
}

static void _on_resume_or_play(Msg::Ptr msg)
{
    _dummy_player_resume_or_play();
    widget_fast_rsp<DummyPlayerResumeOrPlay>(msg);
}

dummy_player::dummy_player()
{
    music_mix = NULL;
    shuffle_mode = DummyPlayerSetShuffleMode::SHUFFLE_ORDER;

    req_dispatcher.set_mode(Msg::TYPE_REQ);
    req_dispatcher.register_handle<DummyPlayerReload>(_on_reload_req);
    req_dispatcher.register_handle<DummyPlayerPause>(_on_pause_req);
    req_dispatcher.register_handle<DummyPlayerNext>(_on_next_req);
    req_dispatcher.register_handle<DummyPlayerSetVolume>(_on_set_volume_req);
    req_dispatcher.register_handle<DummyPlayerSetPosition>(_on_set_position);
    req_dispatcher.register_handle<DummyPlayerSetShuffleMode>(_on_set_shuffle_mode);
    req_dispatcher.register_handle<DummyPlayerResumeOrPlay>(_on_resume_or_play);
}

static void _dummy_player_init(void)
{
    int ret;

    s_player = new dummy_player_t;

    ret = Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MID | MIX_INIT_OPUS |
                   MIX_INIT_WAVPACK);
    if (ret < 0)
    {
        spdlog::critical("Mix_Init failed.");
        exit(EXIT_FAILURE);
    }

    ret = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048);
    if (ret != 0)
    {
        spdlog::critical("Mix_OpenAudio failed.");
        exit(EXIT_FAILURE);
    }

    _soundsphere_dummy_player_set_volume(soundsphere::_config.volume);
    _soundsphere_dummy_player_reload();
}

static void _dummy_player_exit(void)
{
    if (s_player != nullptr)
    {
        _stop_play();

        delete s_player;
        s_player = nullptr;
    }

    Mix_CloseAudio();
}

static void _dummy_player_draw(void)
{
    if (soundsphere::_G.playbar.is_playing && !Mix_PlayingMusic())
    {
        _soundsphere_dummy_player_next();
    }

    /* Update music position. */
    soundsphere::_G.playbar.music_position =
        (s_player->music_mix != NULL) ? Mix_GetMusicPosition(s_player->music_mix) : 0.0;
}

static void _dummy_player_message(Msg::Ptr msg)
{
    s_player->req_dispatcher.dispatch(msg);
}

const soundsphere::widget_t soundsphere::dummy_player = {
    _dummy_player_init,
    _dummy_player_exit,
    _dummy_player_draw,
    _dummy_player_message,
};

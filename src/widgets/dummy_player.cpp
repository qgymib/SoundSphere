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
    soundsphere::MusicTagPtrVecPtr shuffle_vec;

    /**
     * @brief Is in shuffle mode.
     */
    soundsphere::shuffle_mode_t shuffle_mode;
} dummy_player_t;

static dummy_player_t *s_player = nullptr;

dummy_player::dummy_player()
{
    music_mix = NULL;
    shuffle_mode = soundsphere::SHUFFLE_ORDER;
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

    soundsphere::dummy_player_set_volume(soundsphere::_config.volume);
    soundsphere::dummy_player_reload();
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

    soundsphere::dummy_player_set_position(0);
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
        soundsphere::dummy_player_next();
    }

    /* Update music position. */
    soundsphere::_G.playbar.music_position =
        (s_player->music_mix != NULL) ? Mix_GetMusicPosition(s_player->music_mix) : 0.0;
}

const soundsphere::widget_t soundsphere::dummy_player = {
    _dummy_player_init,
    _dummy_player_exit,
    _dummy_player_draw,
};

static soundsphere::MusicTagPtr _find_audio(uint64_t id)
{
    soundsphere::MusicTagPtrVecPtr vec = soundsphere::_G.media_list;

    for (size_t i = 0; i < vec->size(); i++)
    {
        soundsphere::MusicTagPtr obj = vec->at(i);
        if (obj->path_hash == id)
        {
            return obj;
        }
    }

    return soundsphere::MusicTagPtr();
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

void soundsphere::dummy_player_resume_or_play(void)
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
    soundsphere::MusicTagPtr obj = _find_audio(soundsphere::_G.playlist.selected_id);
    if (obj.get() == nullptr)
    {
        return;
    }

    _play(obj);

    if (s_player->shuffle_mode == soundsphere::SHUFFLE_REPEAT)
    {
        soundsphere::dummy_player_set_shuffle(soundsphere::SHUFFLE_REPEAT);
    }
}

void soundsphere::dummy_player_pause(void)
{
    Mix_PauseMusic();
    soundsphere::_G.playbar.is_playing = false;
}

void soundsphere::dummy_player_set_volume(int volume)
{
    int real_volume = (int)(((float)volume / 100.0f) * MIX_MAX_VOLUME);
    Mix_VolumeMusic(real_volume);
}

void soundsphere::dummy_player_set_position(double position)
{
    Mix_SetMusicPosition(position);
}

static soundsphere::MusicTagPtrVecPtr _shuffle_media(soundsphere::MusicTagPtrVecPtr vec)
{
    soundsphere::MusicTagPtrVecPtr ret = std::make_shared<soundsphere::MusicTagPtrVec>(*vec);

    unsigned seed = (unsigned)(soundsphere::clock_time_ms() / 1000);
    std::shuffle(ret->begin(), ret->end(), std::default_random_engine(seed));

    return ret;
}

static void _dummy_player_reshuffle(void)
{
    switch (s_player->shuffle_mode)
    {
    case soundsphere::SHUFFLE_ORDER:
        s_player->shuffle_vec = soundsphere::_G.media_list;
        break;
    case soundsphere::SHUFFLE_REPEAT:
        s_player->shuffle_vec->clear();
        s_player->shuffle_vec->push_back(soundsphere::_G.dummy_player.current_music);
        break;
    case soundsphere::SHUFFLE_RANDOM:
        s_player->shuffle_vec = _shuffle_media(soundsphere::_G.media_list);
        break;
    }
}

void soundsphere::dummy_player_set_shuffle(soundsphere::shuffle_mode_t mode)
{
    s_player->shuffle_mode = mode;
    _dummy_player_reshuffle();
}

void soundsphere::dummy_player_next(void)
{
    size_t idx = 0;
    soundsphere::MusicTagPtrVecPtr vec = s_player->shuffle_vec;

    _stop_play();

    if (vec->empty())
    {
        return;
    }

    for (; idx < vec->size(); idx++)
    {
        soundsphere::MusicTagPtr obj = vec->at(idx);
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

    soundsphere::MusicTagPtr obj = vec->at(idx);
    _play(obj);
}

void soundsphere::dummy_player_reload(void)
{
    _stop_play();
    _dummy_player_reshuffle();
}

soundsphere::shuffle_mode_t soundsphere::dummy_player_get_shuffle_mode(void)
{
    return s_player->shuffle_mode;
}

#include <SDL2/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include "runtime/__init__.hpp"
#include "dummy_player.hpp"
#include "__init__.hpp"

typedef struct dummy_player
{
    dummy_player();

    /**
     * @brief The music we are playing.
     */
    soundsphere::PlayItem::Ptr  music_item;

    /**
     * @brief SDL music instance.
     */
    Mix_Music*                  music_mix;
}dummy_player_t;

static dummy_player_t* s_player = nullptr;

dummy_player::dummy_player()
{
    music_mix = NULL;
}

static void _dummy_player_init(void)
{
    int ret;

    s_player = new dummy_player_t;

    ret = Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MID | MIX_INIT_OPUS | MIX_INIT_WAVPACK);
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

    Mix_VolumeMusic(soundsphere::_G.playbar.volume);
}

static void _stop_play(void)
{
    if (s_player->music_mix != NULL)
    {
        Mix_FreeMusic(s_player->music_mix);
        s_player->music_mix = NULL;
    }

    soundsphere::_G.cover.img.reset();
    soundsphere::_G.lyric.text.clear();

    soundsphere::_G.playbar.is_playing = false;
    soundsphere::_G.playbar.music_position = 0.0;
    soundsphere::_G.playbar.music_duration = 0.0;

    soundsphere::_G.statusbar.bitrate = 0;
    soundsphere::_G.statusbar.samplerate = 0;
    soundsphere::_G.statusbar.channels = 0;
    soundsphere::_G.statusbar.music_type = MUS_NONE;

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
    /* Update music position. */
    soundsphere::_G.playbar.music_position = (s_player->music_mix != NULL) ?
        Mix_GetMusicPosition(s_player->music_mix) : 0.0;
}

const soundsphere::widget_t soundsphere::dummy_player = {
_dummy_player_init,
_dummy_player_exit,
_dummy_player_draw,
};

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

    s_player->music_item = soundsphere::_G.media_list->at(soundsphere::_G.playlist.selected_idx);
    s_player->music_mix = Mix_LoadMUS(s_player->music_item->path.c_str());

    Mix_PlayMusic(s_player->music_mix, 1);

    s_player->music_item->compile_cover();
    soundsphere::_G.cover.img = s_player->music_item->cover_texture;

    soundsphere::_G.lyric.text = s_player->music_item->lyric;

    soundsphere::_G.playbar.is_playing = true;
    soundsphere::_G.playbar.music_duration = Mix_MusicDuration(s_player->music_mix);
    soundsphere::_G.playbar.music_position = 0.0;

    soundsphere::_G.statusbar.music_type = Mix_GetMusicType(s_player->music_mix);
    soundsphere::_G.statusbar.bitrate = s_player->music_item->bitrate;
    soundsphere::_G.statusbar.samplerate = s_player->music_item->samplerate;
    soundsphere::_G.statusbar.channels = s_player->music_item->channels;
}

void soundsphere::dummy_player_pause(void)
{
    Mix_PauseMusic();

    soundsphere::_G.playbar.is_playing = false;
}

void soundsphere::dummy_player_set_volume(int volume)
{
    Mix_VolumeMusic(volume);
}

void soundsphere::dummy_player_set_position(double position)
{
    Mix_SetMusicPosition(position);
}

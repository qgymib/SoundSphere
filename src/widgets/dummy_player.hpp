#ifndef SOUND_SPHERE_WIDGETS_DUMMY_PLAYER_HPP
#define SOUND_SPHERE_WIDGETS_DUMMY_PLAYER_HPP

namespace soundsphere
{

typedef enum shuffle_mode_e
{
    SHUFFLE_ORDER,
    SHUFFLE_RANDOM,
    SHUFFLE_REPEAT,
} shuffle_mode_t;

void dummy_player_reload(void);

/**
 * @brief Play or resume audio.
 * If audio is paused, resume the audio.
 * If audio is playing, re-play the audio.
 * If audio is not playing, play the selected item.
 */
void dummy_player_resume_or_play(void);

/**
 * @brief Pause the audio.
 */
void dummy_player_pause(void);

/**
 * @brief Set volume.
 */
void dummy_player_set_volume(int volume);

/**
 * @brief Set current playing position.
 */
void dummy_player_set_position(double position);

/**
 * @brief Shuffle musics.
 */
void dummy_player_set_shuffle(shuffle_mode_t mode);

shuffle_mode_t dummy_player_get_shuffle_mode(void);

/**
 * @brief Next music.
 */
void dummy_player_next(void);

} // namespace soundsphere

#endif

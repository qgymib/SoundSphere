#ifndef SOUND_SPHERE_UTILS_MUSIC_TAG_HPP
#define SOUND_SPHERE_UTILS_MUSIC_TAG_HPP

#include <string>
#include <memory>
#include "utils/binary.hpp"

namespace soundsphere {

typedef struct music_tags
{
    music_tags();

    /**
     * @brief Music path in UTF-8 encoding.
     */
    std::string path;

    /**
     * @brief Title in UTF-8 encoding.
     */
    std::string title;

    /**
     * @brief Artist in UTF-8 encoding.
     */
    std::string artist;

    /**
     * @brief Lyric in UTF-8 encoding.
     */
    std::string lyric;

    /**
     * @brief Vector of cover binary data.
     */
    BinVec      covers;

    /**
     * @brief Bitrate in kb/s.
     */
    int         bitrate;

    /**
     * @brief Sample rate in Hz.
     */
    int         samplerate;

    /**
     * @brief The number of audio channels.
     */
    int         channel;

    /**
     * @brief The hash value of path.
     */
    uint64_t    path_hash;
} music_tags_t;

typedef std::shared_ptr<music_tags_t> MusicTagPtr;

/**
 * @brief Read music tags.
 * @param[in,out] tags Tags. The #music_tags_t::path field must be filled first.
 * @param[out] errinfo  Error information
 * @return  Boolean.
 */
bool music_read_tag(music_tags_t& tags, std::string& errinfo);

/**
 * @brief Write music tags.
 * @param[in] tags Tags.
 * @param[out] effinfo  Error information.
 * @return Boolean.
 */
bool music_write_tag(const music_tags_t& tags, std::string& errinfo);

}

#endif

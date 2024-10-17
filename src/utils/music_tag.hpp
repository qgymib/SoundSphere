#ifndef SOUND_SPHERE_UTILS_MUSIC_TAG_HPP
#define SOUND_SPHERE_UTILS_MUSIC_TAG_HPP

#include <memory>
#include <vector>
#include "utils/binary.hpp"
#include "utils/string.hpp"

namespace soundsphere
{

typedef enum music_type
{
    MUSIC_NONE,
    MUSIC_FLAC,
    MUSIC_MP3,
} music_type_t;

typedef struct music_tag_image
{
    /**
     * @brief `image/png` or `image/jpeg`.
     */
    std::string mime;

    /**
     * @brief Image data.
     */
    Bin data;
} music_tag_image_t;

typedef std::vector<music_tag_image_t> ImageVec;

typedef struct music_tags_info
{
    music_tags_info();

    /**
     * @brief Music format.
     */
    music_type_t format;

    /**
     * @brief Bitrate in kb/s.
     */
    int bitrate;

    /**
     * @brief Sample rate in Hz.
     */
    int samplerate;

    /**
     * @brief The number of audio channels.
     */
    int channel;

    /**
     * @brief The length of audio in seconds.
     */
    double duration;

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
    ImageVec covers;
} music_tags_info_t;

typedef struct music_tags
{
    music_tags();

    /**
     * @brief The hash value of path.
     */
    uint64_t path_hash;

    /**
     * @brief Music path in UTF-8 encoding.
     */
    std::string path;

    /**
     * @brief Whether this is a valid tags.
     */
    bool valid;

    /**
     * @brief Error information if #valid is false.
     */
    std::string errinfo;

    /**
     * @brief Music tags if #valid is true.
     */
    music_tags_info_t info;
} music_tags_t;

/**
 * @breif Smart pointer types for #music_tags_t.
 * @{
 */
typedef std::shared_ptr<music_tags_t> MusicTagPtr;
typedef std::vector<MusicTagPtr> MusicTagPtrVec;
typedef std::shared_ptr<MusicTagPtrVec> MusicTagPtrVecPtr;
/**
 * @}
 */

/**
 * @brief Get format string.
 * @param[in] format    Format type.
 * @return  C string. For unknown type, NULL is returned.
 */
const char *music_tag_format_name(music_type_t format);

/**
 * @brief Read music tags.
 * @param[in,out] tags Tags. The #music_tags_t::path field must be filled first.
 * @param[out] errinfo  Error information
 * @return  Boolean.
 */
bool music_read_tag(music_tags_t &tags);

/**
 * @brief Read batch of path and return their tags.
 * @param[in] path  Path list.
 * @return          Music tags for each file.
 */
MusicTagPtrVecPtr music_read_tag_v(const StringVec &paths);

/**
 * @brief Write music tags.
 * @param[in] tags Tags.
 * @param[out] effinfo  Error information.
 * @return Boolean.
 */
bool music_write_tag(const music_tags_t &tags, std::string &errinfo);

} // namespace soundsphere

#endif

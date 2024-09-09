#ifndef SOUND_SPHERE_RUNTIME_PLAYITEM_HPP
#define SOUND_SPHERE_RUNTIME_PLAYITEM_HPP

#include <memory>
#include <string>
#include <vector>
#include <taglib/tbytevector.h>
#include "backends/__init__.hpp"

namespace soundsphere {

class PlayItem
{
public:
    typedef std::shared_ptr<PlayItem> Ptr;
    typedef std::vector<Ptr> PtrVec;
    typedef std::shared_ptr<PtrVec> PtrVecPtr;

public:
    /* Use #soundsphere::PlayItem::make(). */
    PlayItem();
    virtual ~PlayItem();

public:
    /**
     * @brief Make a new item.
     * @param[in] path  Path to music item.
     * @return boolean.
     */
    static bool make(Ptr& obj, const std::string& path);

public:
    /**
     * @brief Load cover.
     * @note Must call this function in UI thread.
     */
    void compile_cover(void);

public:
    /**
     * @brief Unique id.
     */
    uint64_t uid;

    /**
     * @brief Path to music file.
     */
    std::string path;

    /**
     * @brief Extension name.
     */
    std::string ext;

    /**
     * @brief Title of this music.
     */
    std::string title;

    /**
     * @brief Artist of this music.
     */
    std::string artist;

    /**
     * @brief Lyric.
     */
    std::string lyric;

    /**
     * @brief Cover data, which is a image.
     */
    TagLib::ByteVector cover_data;

    /**
     * @brief Compiled cover texture.
     */
    soundsphere::Texture cover_texture;

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
    int channels;
};

}

#endif

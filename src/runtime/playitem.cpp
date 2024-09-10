#include <imgui.h>
#include <taglib/flacfile.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/unsynchronizedlyricsframe.h>
#include <taglib/xiphcomment.h>
#include "utils/path.hpp"
#include "utils/string.hpp"
#include "playitem.hpp"

typedef struct file_prop_record
{
    /**
     * @brief Extension name.
     */
    const char* ext;

    /**
     * @brief Load properties.
     */
    bool (*get_properties)(soundsphere::PlayItem* obj);
} file_prop_record_t;

template<typename T>
static std::shared_ptr<T> _access_file(const char* path)
{
#if defined(_WIN32)
    soundsphere::wstring path_w = soundsphere::utf8_to_wide(path);
    std::shared_ptr<T> file = std::make_shared<T>(path_w.get());
#else
    std::shared_ptr<T> file = std::make_shared<T>(path);
#endif

    return file;
}

template<typename T>
static void _playitem_get_properties_common(soundsphere::PlayItem* obj,
    std::shared_ptr<T> file)
{
    obj->title = file->tag()->title().to8Bit(true);
    obj->artist = file->tag()->artist().to8Bit(true);
    obj->bitrate = file->audioProperties()->bitrate();
    obj->samplerate = file->audioProperties()->sampleRate();
    obj->channels = file->audioProperties()->channels();
}

template<typename T>
static bool _playitem_load_cover_from_id3v2(soundsphere::PlayItem* obj,
    std::shared_ptr<T> file)
{
    TagLib::ID3v2::Tag* tag = file->ID3v2Tag();
    if (tag == nullptr)
    {
        return false;
    }

    TagLib::ID3v2::FrameList frames = tag->frameListMap()["APIC"];
    if (frames.isEmpty())
    {
        return false;
    }

    TagLib::ID3v2::AttachedPictureFrame* cover =
        static_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
    obj->cover_data = cover->picture();

    return true;
}

template<typename T>
static bool _playitem_load_lyric_from_id3v2(soundsphere::PlayItem* obj,
    std::shared_ptr<T> file)
{
    TagLib::ID3v2::Tag* tag = file->ID3v2Tag();
    if (tag == nullptr)
    {
        return false;
    }

    TagLib::ID3v2::FrameList frames = tag->frameListMap()["USLT"];
    if (frames.isEmpty())
    {
        return false;
    }

    TagLib::ID3v2::UnsynchronizedLyricsFrame* lyricsFrame =
        static_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(frames.front());
    obj->lyric = lyricsFrame->text().to8Bit(true);

    return true;
}

static bool _playitem_get_lyric_flac(soundsphere::PlayItem* obj,
    std::shared_ptr<TagLib::FLAC::File> file)
{
    TagLib::Ogg::XiphComment* tag = file->xiphComment();
    if (tag == nullptr)
    {
        return false;
    }

    TagLib::String lyric = tag->fieldListMap()["LYRICS"].toString();
    if (lyric.isEmpty())
    {
        return false;
    }

    obj->lyric = lyric.to8Bit(true);
    return true;
}

static bool _playitem_get_properties_flac(soundsphere::PlayItem* obj)
{
    std::shared_ptr<TagLib::FLAC::File> file = _access_file<TagLib::FLAC::File>(obj->path.c_str());
    if (!file->isValid())
    {
        return false;
    }

    _playitem_get_properties_common(obj, file);
    _playitem_get_lyric_flac(obj, file);

    auto pict_list = file->pictureList();
    if (pict_list.size() != 0)
    {
        TagLib::FLAC::Picture* p = pict_list[0];
        obj->cover_data = p->data();
    }

    return true;
}

static bool _playitem_get_properties_mp3(soundsphere::PlayItem* obj)
{
    std::shared_ptr<TagLib::MPEG::File> file = _access_file<TagLib::MPEG::File>(obj->path.c_str());
    if (!file->isValid())
    {
        return false;
    }

    _playitem_get_properties_common(obj, file);
    _playitem_load_lyric_from_id3v2(obj, file);

    return _playitem_load_cover_from_id3v2(obj, file);
}

static file_prop_record_t s_format_map[] = {
    { ".flac", _playitem_get_properties_flac },
    { ".mp3",  _playitem_get_properties_mp3 },
};

soundsphere::PlayItem::PlayItem()
{
    static uint64_t s_uid = 0;

    uid = s_uid++;
    bitrate = 0;
    samplerate = 0;
    channels = 0;
}

soundsphere::PlayItem::~PlayItem()
{
}

bool soundsphere::PlayItem::make(soundsphere::PlayItem::Ptr& obj,
    const std::string& path)
{
    obj = std::make_shared<PlayItem>();
    obj->path = path;
    obj->ext = soundsphere::extname(path);

    bool processed = false;
    for (size_t i = 0; i < IM_ARRAYSIZE(s_format_map); i++)
    {
        file_prop_record_t* rec = &s_format_map[i];
        if (strcmp(rec->ext, obj->ext.c_str()) == 0)
        {
            processed = rec->get_properties(obj.get());
            break;
        }
    }
    if (!processed)
    {
        return false;
    }

    /*
     * Fixup
     */
    if (obj->title.empty())
    {
        obj->title = soundsphere::basename(path);
    }

    return true;
}

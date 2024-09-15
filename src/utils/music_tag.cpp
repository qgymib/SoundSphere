#include <cstring>
#include <memory>
#include <taglib/tfilestream.h>
#include <taglib/flacfile.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/unsynchronizedlyricsframe.h>
#include <taglib/xiphcomment.h>
#include "utils/defines.hpp"
#include "utils/path.hpp"
#include "utils/string.hpp"
#include "music_tag.hpp"

typedef struct tag_op_item
{
    /**
     * @brief Extension name.
     */
    const char* ext;

    /**
     * @brief Tags read function.
     */
    bool (*read_tag_fn)(soundsphere::music_tags_t& tags,
        std::string& errinfo);
    
    /**
     * @brief Tags write function.
     */
    bool (*write_tag_fn)(const soundsphere::music_tags_t& tags,
        std::string& errinfo);
} tag_ops_item_t;

template<typename T>
struct MusicFile
{
    ~MusicFile()
    {
        delete handle;
        delete stream;
    }

    T*                  handle; /**< File handle. */
    TagLib::IOStream*   stream; /**< Stream handle. */
    bool                rdonly; /**< Open as readonly. */
};

static void _taglib_binary_to_std_vector(soundsphere::Bin& dst,
    const TagLib::ByteVector& src)
{
    size_t src_sz = src.size();
    dst.resize(src_sz);

    const void* src_data = src.data();
    void* dst_data = dst.data();
    memcpy(dst_data, src_data, src_sz);
}

static void _std_vector_to_taglib_binary(TagLib::ByteVector& dst,
    const soundsphere::Bin& src)
{
    const uint8_t* data = src.data();
    size_t data_sz = src.size();
    dst = TagLib::ByteVector((char*)data, (unsigned)data_sz);
}

template<typename T>
static std::shared_ptr< MusicFile<T> > _access_file(const std::string& path,
    bool rdonly)
{
#if defined(_WIN32)
    soundsphere::wstring path_w = soundsphere::utf8_to_wide(path.c_str());
    TagLib::FileName fname = path_w.get();
#else
    TagLib::FileName fname = path.c_str();
#endif

    std::shared_ptr< MusicFile<T> > file = std::make_shared< MusicFile<T> >();
    file->stream = new TagLib::FileStream(fname, rdonly);
    file->handle = new T(file->stream);

    return file;
}

template<typename T>
static void _tag_reader_get_properties_common(T* file,
    soundsphere::music_tags_t& tags)
{
    tags.title = file->tag()->title().to8Bit(true);
    tags.artist = file->tag()->artist().to8Bit(true);
    tags.bitrate = file->audioProperties()->bitrate();
    tags.samplerate = file->audioProperties()->sampleRate();
    tags.channel = file->audioProperties()->channels();
}

template<typename T>
static void _tag_writer_set_properties_common(T* file,
    const soundsphere::music_tags_t& tags)
{
    file->tag()->setTitle(tags.title);
    file->tag()->setArtist(tags.artist);
}

template<typename T>
static bool _tag_reader_get_lyric_id3v2(T* file,
    soundsphere::music_tags_t& tags)
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
    tags.lyric = lyricsFrame->text().to8Bit(true);

    return true;
}

template<typename T>
static bool _tag_writer_set_lyric_id3v2(T* file,
    const soundsphere::music_tags_t& tags)
{
    TagLib::ID3v2::UnsynchronizedLyricsFrame* lyric_frame =
        new TagLib::ID3v2::UnsynchronizedLyricsFrame(TagLib::String::UTF8);
    lyric_frame->setText(tags.lyric);

    TagLib::ID3v2::Tag* tag = file->ID3v2Tag(true);
    tag->removeFrames("USLT");
    tag->addFrame(lyric_frame);

    return true;
}

template<typename T>
static bool _tag_reader_get_cover_id3v2(T* file,
    soundsphere::music_tags_t& tags)
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

    TagLib::ID3v2::AttachedPictureFrame* cover_raw =
        static_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
    soundsphere::Bin cover;
    _taglib_binary_to_std_vector(cover, cover_raw->picture());
    tags.covers.push_back(cover);

    return true;
}

template<typename T>
static bool _tag_writer_set_cover_id3v2(T* file,
    const soundsphere::music_tags_t& tags)
{
    if (tags.covers.size() == 0)
    {
        return false;
    }

    TagLib::ByteVector picture;
    _std_vector_to_taglib_binary(picture, tags.covers[0]);

    TagLib::ID3v2::AttachedPictureFrame* cover_frame = new TagLib::ID3v2::AttachedPictureFrame;
    cover_frame->setPicture(picture);

    TagLib::ID3v2::Tag* tag = file->ID3v2Tag(true);
    tag->removeFrames("APIC");
    tag->addFrame(cover_frame);

    return true;
}

static bool _mp3_tag_reader(soundsphere::music_tags_t& tags,
    std::string& errinfo)
{
    auto file = _access_file<TagLib::MPEG::File>(tags.path, true);
    if (!file->handle->isValid())
    {
        errinfo = soundsphere::string_format("%s is invalid", tags.path.c_str());
        return false;
    }

    _tag_reader_get_properties_common(file->handle, tags);
    _tag_reader_get_lyric_id3v2(file->handle, tags);
    _tag_reader_get_cover_id3v2(file->handle, tags);

    return true;
}

static bool _mp3_tag_writer(const soundsphere::music_tags_t& tags,
    std::string& errinfo)
{
    auto file = _access_file<TagLib::FLAC::File>(tags.path, false);
    if (!file->handle->isValid())
    {
        errinfo = soundsphere::string_format("%s is invalid", tags.path.c_str());
        return false;
    }

    _tag_writer_set_properties_common(file->handle, tags);
    _tag_writer_set_lyric_id3v2(file->handle, tags);
    _tag_writer_set_cover_id3v2(file->handle, tags);

    return file->handle->save();
}

static bool _flac_get_lyric(TagLib::FLAC::File* file,
    soundsphere::music_tags_t& tags)
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

    tags.lyric = lyric.to8Bit(true);
    return true;
}

static void _flac_set_lyric(TagLib::FLAC::File* file,
    const soundsphere::music_tags_t& tags)
{
    TagLib::Ogg::XiphComment* tag = file->xiphComment(true);
    tag->addField("LYRICS", tags.lyric);
}

static bool _flac_get_cover(TagLib::FLAC::File* file,
    soundsphere::music_tags_t& tags)
{
    auto picture_list = file->pictureList();
    size_t picture_list_sz = picture_list.size();
    if (picture_list_sz == 0)
    {
        return false;
    }

    TagLib::List<TagLib::FLAC::Picture*>::Iterator it = picture_list.begin();
    for (; it != picture_list.end(); it++)
    {
        soundsphere::Bin cover;
        TagLib::FLAC::Picture* picture = *it;
        _taglib_binary_to_std_vector(cover, picture->data());
        tags.covers.push_back(cover);
    }

    return true;
}

static void _flac_set_cover(TagLib::FLAC::File* file,
    const soundsphere::music_tags_t& tags)
{
    file->removePictures();

    soundsphere::BinVec::const_iterator it = tags.covers.begin();
    for (; it != tags.covers.end(); it++)
    {
        TagLib::ByteVector cover;
        _std_vector_to_taglib_binary(cover, *it);

        TagLib::FLAC::Picture* picture = new TagLib::FLAC::Picture(cover);
        file->addPicture(picture);
    }
}

static bool _flac_tag_reader(soundsphere::music_tags_t& tags,
    std::string& errinfo)
{
    auto file = _access_file<TagLib::FLAC::File>(tags.path, true);
    if (!file->handle->isValid())
    {
        errinfo = soundsphere::string_format("%s is invalid", tags.path.c_str());
        return false;
    }

    _tag_reader_get_properties_common(file->handle, tags);
    _flac_get_lyric(file->handle, tags);
    _flac_get_cover(file->handle, tags);

    return true;
}

static bool _flac_tag_writer(const soundsphere::music_tags_t& tags,
    std::string& errinfo)
{
    auto file = _access_file<TagLib::FLAC::File>(tags.path, false);
    if (!file->handle->isValid())
    {
        errinfo = soundsphere::string_format("%s is invalid", tags.path.c_str());
        return false;
    }

    _tag_writer_set_properties_common(file->handle, tags);
    _flac_set_lyric(file->handle, tags);
    _flac_set_cover(file->handle, tags);

    return file->handle->save();
}

static const tag_ops_item_t s_tag_op_table[] = {
    { ".flac",  _flac_tag_reader,   _flac_tag_writer },
    { ".mp3",   _mp3_tag_reader,    _mp3_tag_writer },
};

soundsphere::music_tags::music_tags()
{
    bitrate = 0;
    samplerate = 0;
    channel = 0;
    path_hash = 0;
}

bool soundsphere::music_read_tag(soundsphere::music_tags_t& tags,
    std::string& errinfo)
{
    std::string extname = soundsphere::extname(tags.path);

    bool ret = false;
    for (size_t i = 0; i < ARRAY_SIZE(s_tag_op_table); i++)
    {
        const tag_ops_item_t* reader = &s_tag_op_table[i];
        if (extname == reader->ext)
        {
            ret = reader->read_tag_fn(tags, errinfo);
            goto finish_fill_title_if_empty;
        }
    }

    errinfo = soundsphere::string_format("%s: %s",
        "Unknown music format", extname.c_str());
    return false;

finish_fill_title_if_empty:
    if (ret && tags.title.empty())
    {
        tags.title = soundsphere::basename(tags.path, false);
    }
    tags.path_hash = soundsphere::string_hash_djb2(tags.path);
    return ret;
}

bool soundsphere::music_write_tag(const soundsphere::music_tags_t& tags,
    std::string& errinfo)
{
    std::string extname = soundsphere::extname(tags.path);

    for (size_t i = 0; i < ARRAY_SIZE(s_tag_op_table); i++)
    {
        const tag_ops_item_t* writer = &s_tag_op_table[i];
        if (extname == writer->ext)
        {
            return writer->write_tag_fn(tags, errinfo);
        }
    }

    errinfo = soundsphere::string_format("%s: %s",
        "Unknown music format", extname.c_str());
    return false;
}

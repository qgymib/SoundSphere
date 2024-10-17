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
     * @brief Music format.
     */
    soundsphere::music_type_t format;

    /**
     * @brief Extension name.
     */
    const char *ext;

    /**
     * @brief Tags read function.
     */
    bool (*read_tag_fn)(soundsphere::music_tags_t &tags, std::string &errinfo);

    /**
     * @brief Tags write function.
     */
    bool (*write_tag_fn)(const soundsphere::music_tags_t &tags, std::string &errinfo);
} tag_ops_item_t;

template <typename T> struct MusicFile
{
    ~MusicFile()
    {
        delete handle;
        delete stream;
    }

    T *handle;                /**< File handle. */
    TagLib::IOStream *stream; /**< Stream handle. */
    bool rdonly;              /**< Open as readonly. */
};

static void _taglib_binary_to_std_vector(soundsphere::Bin &dst, const TagLib::ByteVector &src)
{
    size_t src_sz = src.size();
    dst.resize(src_sz);

    const void *src_data = src.data();
    void *dst_data = dst.data();
    memcpy(dst_data, src_data, src_sz);
}

static void _std_vector_to_taglib_binary(TagLib::ByteVector &dst, const soundsphere::Bin &src)
{
    const uint8_t *data = src.data();
    size_t data_sz = src.size();
    dst = TagLib::ByteVector((char *)data, (unsigned)data_sz);
}

template <typename T> static std::shared_ptr<MusicFile<T>> _access_file(const std::string &path, bool rdonly)
{
#if defined(_WIN32)
    soundsphere::wstring path_w = soundsphere::utf8_to_wide(path.c_str());
    TagLib::FileName fname = path_w.get();
#else
    TagLib::FileName fname = path.c_str();
#endif

    std::shared_ptr<MusicFile<T>> file = std::make_shared<MusicFile<T>>();
    file->stream = new TagLib::FileStream(fname, rdonly);
    file->handle = new T(file->stream);

    return file;
}

template <typename T> static void _tag_reader_get_properties_common(T *file, soundsphere::music_tags_t &tags)
{
    tags.info.title = file->tag()->title().to8Bit(true);
    tags.info.artist = file->tag()->artist().to8Bit(true);
    tags.info.bitrate = file->audioProperties()->bitrate();
    tags.info.samplerate = file->audioProperties()->sampleRate();
    tags.info.channel = file->audioProperties()->channels();

    int length = file->audioProperties()->lengthInMilliseconds();
    tags.info.duration = (double)length / 1000.0;
}

template <typename T> static void _tag_writer_set_properties_common(T *file, const soundsphere::music_tags_t &tags)
{
    TagLib::String title(tags.info.title, TagLib::String::UTF8);
    file->tag()->setTitle(title);

    TagLib::String artist(tags.info.artist, TagLib::String::UTF8);
    file->tag()->setArtist(artist);
}

template <typename T> static bool _tag_reader_get_lyric_id3v2(T *file, soundsphere::music_tags_t &tags)
{
    TagLib::ID3v2::Tag *tag = file->ID3v2Tag();
    if (tag == nullptr)
    {
        return false;
    }

    TagLib::ID3v2::FrameList frames = tag->frameListMap()["USLT"];
    if (frames.isEmpty())
    {
        return false;
    }

    TagLib::ID3v2::UnsynchronizedLyricsFrame *lyricsFrame =
        static_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame *>(frames.front());
    tags.info.lyric = lyricsFrame->text().to8Bit(true);

    return true;
}

template <typename T> static bool _tag_writer_set_lyric_id3v2(T *file, const soundsphere::music_tags_t &tags)
{
    TagLib::String lyric(tags.info.lyric, TagLib::String::UTF8);

    TagLib::ID3v2::UnsynchronizedLyricsFrame *lyric_frame =
        new TagLib::ID3v2::UnsynchronizedLyricsFrame(TagLib::String::UTF8);
    lyric_frame->setText(lyric);

    TagLib::ID3v2::Tag *tag = file->ID3v2Tag(true);
    tag->removeFrames("USLT");
    tag->addFrame(lyric_frame);

    return true;
}

template <typename T> static bool _tag_reader_get_cover_id3v2(T *file, soundsphere::music_tags_t &tags)
{
    TagLib::ID3v2::Tag *tag = file->ID3v2Tag();
    if (tag == nullptr)
    {
        return false;
    }

    TagLib::ID3v2::FrameList frames = tag->frameListMap()["APIC"];
    if (frames.isEmpty())
    {
        return false;
    }

    TagLib::ID3v2::AttachedPictureFrame *cover_raw = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front());
    soundsphere::music_tag_image_t cover;
    _taglib_binary_to_std_vector(cover.data, cover_raw->picture());
    cover.mime = cover_raw->mimeType().to8Bit(true);
    tags.info.covers.push_back(cover);

    return true;
}

template <typename T> static bool _tag_writer_set_cover_id3v2(T *file, const soundsphere::music_tags_t &tags)
{
    if (tags.info.covers.size() == 0)
    {
        return false;
    }

    const soundsphere::music_tag_image_t &music_picture = tags.info.covers[0];

    TagLib::ByteVector picture;
    _std_vector_to_taglib_binary(picture, music_picture.data);

    TagLib::ID3v2::AttachedPictureFrame *cover_frame = new TagLib::ID3v2::AttachedPictureFrame;
    cover_frame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);

    TagLib::String mime(music_picture.mime, TagLib::String::UTF8);
    cover_frame->setMimeType(mime);

    cover_frame->setDescription("Cover");
    cover_frame->setPicture(picture);

    TagLib::ID3v2::Tag *tag = file->ID3v2Tag(true);
    tag->removeFrames("APIC");
    tag->addFrame(cover_frame);

    return true;
}

static bool _mp3_tag_reader(soundsphere::music_tags_t &tags, std::string &errinfo)
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

static bool _mp3_tag_writer(const soundsphere::music_tags_t &tags, std::string &errinfo)
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

static bool _flac_get_lyric(TagLib::FLAC::File *file, soundsphere::music_tags_t &tags)
{
    TagLib::Ogg::XiphComment *tag = file->xiphComment();
    if (tag == nullptr)
    {
        return false;
    }

    TagLib::String lyric = tag->fieldListMap()["LYRICS"].toString();
    if (lyric.isEmpty())
    {
        return false;
    }

    tags.info.lyric = lyric.to8Bit(true);
    return true;
}

static void _flac_set_lyric(TagLib::FLAC::File *file, const soundsphere::music_tags_t &tags)
{
    TagLib::String lyric(tags.info.lyric, TagLib::String::UTF8);

    TagLib::Ogg::XiphComment *tag = file->xiphComment(true);
    tag->addField("LYRICS", lyric);
}

static bool _flac_get_cover(TagLib::FLAC::File *file, soundsphere::music_tags_t &tags)
{
    auto picture_list = file->pictureList();
    size_t picture_list_sz = picture_list.size();
    if (picture_list_sz == 0)
    {
        return false;
    }

    TagLib::List<TagLib::FLAC::Picture *>::Iterator it = picture_list.begin();
    for (; it != picture_list.end(); it++)
    {
        soundsphere::music_tag_image_t cover;
        TagLib::FLAC::Picture *picture = *it;
        _taglib_binary_to_std_vector(cover.data, picture->data());
        cover.mime = picture->mimeType().to8Bit(true);
        tags.info.covers.push_back(cover);
    }

    return true;
}

static void _flac_set_cover(TagLib::FLAC::File *file, const soundsphere::music_tags_t &tags)
{
    file->removePictures();

    soundsphere::ImageVec::const_iterator it = tags.info.covers.begin();
    for (; it != tags.info.covers.end(); it++)
    {
        const soundsphere::music_tag_image_t &music_image = *it;

        TagLib::ByteVector cover;
        _std_vector_to_taglib_binary(cover, music_image.data);

        TagLib::FLAC::Picture *picture = new TagLib::FLAC::Picture;
        picture->setType(TagLib::FLAC::Picture::FrontCover);

        TagLib::String mime(music_image.mime, TagLib::String::UTF8);
        picture->setMimeType(mime);

        picture->setDescription("Cover");
        picture->setData(cover);
        file->addPicture(picture);
    }
}

static bool _flac_tag_reader(soundsphere::music_tags_t &tags, std::string &errinfo)
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

static bool _flac_tag_writer(const soundsphere::music_tags_t &tags, std::string &errinfo)
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
    { soundsphere::MUSIC_FLAC, ".flac", _flac_tag_reader, _flac_tag_writer },
    { soundsphere::MUSIC_MP3,  ".mp3",  _mp3_tag_reader,  _mp3_tag_writer  },
};

soundsphere::music_tags::music_tags()
{
    path_hash = 0;
    valid = false;
}

soundsphere::music_tags_info::music_tags_info()
{
    format = MUSIC_NONE;
    bitrate = 0;
    samplerate = 0;
    channel = 0;
    duration = 0.0;
}

const char *soundsphere::music_tag_format_name(music_type_t format)
{
    switch (format)
    {
    case MUSIC_FLAC:
        return "FLAC";
    case MUSIC_MP3:
        return "MP3";
    default:
        break;
    }
    return nullptr;
}

bool soundsphere::music_read_tag(soundsphere::music_tags_t &tags)
{
    for (size_t i = 0; i < ARRAY_SIZE(s_tag_op_table); i++)
    {
        const tag_ops_item_t *reader = &s_tag_op_table[i];
        if (soundsphere::string_last_match(tags.path, reader->ext))
        {
            tags.info.format = reader->format;
            tags.valid = reader->read_tag_fn(tags, tags.errinfo);
            goto finish_fill_title_if_empty;
        }
    }

    tags.valid = false;
    tags.errinfo = soundsphere::string_format("%s: %s", "Unknown music format", tags.path.c_str());
    return false;

finish_fill_title_if_empty:
    if (tags.valid && tags.info.title.empty())
    {
        tags.info.title = soundsphere::basename(tags.path, false);
    }
    tags.path_hash = soundsphere::string_hash_djb2(tags.path);
    return tags.valid;
}

bool soundsphere::music_write_tag(const soundsphere::music_tags_t &tags, std::string &errinfo)
{
    std::string extname = soundsphere::extname(tags.path);

    for (size_t i = 0; i < ARRAY_SIZE(s_tag_op_table); i++)
    {
        const tag_ops_item_t *writer = &s_tag_op_table[i];
        if (extname == writer->ext)
        {
            return writer->write_tag_fn(tags, errinfo);
        }
    }

    errinfo = soundsphere::string_format("%s: %s", "Unknown music format", extname.c_str());
    return false;
}

soundsphere::MusicTagPtrVecPtr soundsphere::music_read_tag_v(const StringVec &paths)
{
    soundsphere::MusicTagPtrVecPtr vec = std::make_shared<soundsphere::MusicTagPtrVec>();

    for (size_t i = 0; i < paths.size(); i++)
    {
        soundsphere::MusicTagPtr obj = std::make_shared<soundsphere::music_tags_t>();
        obj->path = paths[i];

        soundsphere::music_read_tag(*obj);
        vec->push_back(obj);
    }

    return vec;
}

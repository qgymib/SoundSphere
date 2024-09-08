#include <taglib/fileref.h>
#include "utils/path.hpp"
#include "utils/win32.hpp"
#include "playitem.hpp"

soundsphere::PlayItem::PlayItem()
{
}

soundsphere::PlayItem::~PlayItem()
{
}

bool soundsphere::PlayItem::make(soundsphere::PlayItem::Ptr& obj,const std::string& path)
{
    /*
     * In windows, TagLib except the path is encoding in local page.
     * Due to \p path is always in UTF-8, we convert to wchar_t* to make this working.
     */
#if defined(_WIN32)
    soundsphere::wstring path_w = soundsphere::utf8_to_wide(path.c_str());
    TagLib::FileRef f(path_w.get());
#else
    TagLib::FileRef f(path.c_str());
#endif

    if (f.isNull())
    {
        return false;
    }

    obj = std::make_shared<PlayItem>();
    obj->path = path;
    obj->title = f.tag()->title().to8Bit(true);
    obj->artist = f.tag()->artist().to8Bit(true);
    obj->bitrate = f.audioProperties()->bitrate();
    obj->samplerate = f.audioProperties()->sampleRate();
    obj->channels = f.audioProperties()->channels();

    /*
     * Fixup
     */
    if (obj->title.empty())
    {
        obj->title = soundsphere::basename(path);
    }

    return true;
}

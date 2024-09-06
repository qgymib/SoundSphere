#include <taglib/fileref.h>
#include "utils/path.hpp"
#include "playitem.hpp"

soundsphere::PlayItem::PlayItem()
{
}

soundsphere::PlayItem::~PlayItem()
{
}

bool soundsphere::PlayItem::make(soundsphere::PlayItem::Ptr& obj,const std::string& path)
{
    TagLib::FileRef f(path.c_str());
    if (f.isNull())
    {
        return false;
    }

    obj = std::make_shared<PlayItem>();
    obj->path = path;
    obj->title = f.tag()->title().to8Bit(true);
    obj->artist = f.tag()->artist().to8Bit(true);

    /*
     * Fixup
     */
    if (obj->title.empty())
    {
        obj->title = soundsphere::basename(path);
    }

    return true;
}

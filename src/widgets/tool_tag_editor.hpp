#ifndef SOUND_SPHERE_WIDGETS_TOOL_TAG_EDITOR_HPP
#define SOUND_SPHERE_WIDGETS_TOOL_TAG_EDITOR_HPP

#include <string>
#include "__init__.hpp"

namespace soundsphere
{

struct TagEditorOpen
{
    const static uint64_t ID = MAKE_MSGID(WIDGET_ID_TOOL_TAG_EDITOR, __LINE__);

    struct Req : public Msg::Req
    {
        Req(const std::string &path);
        std::string path;
    };

    struct Rsp : public Msg::Rsp
    {
    };
};

} // namespace soundsphere

#endif

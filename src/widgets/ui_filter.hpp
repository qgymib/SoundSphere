#ifndef SOUND_SPHERE_WIDGETS_UI_FILTER_HPP
#define SOUND_SPHERE_WIDGETS_UI_FILTER_HPP

#include "__init__.hpp"

namespace soundsphere
{

struct UiFilterReset
{
    const static uint64_t ID = MAKE_MSGID(WIDGET_ID_UI_FILTER, __LINE__);

    struct Req : public Msg::Req
    {
    };

    struct Rsp : public Msg::Rsp
    {
    };
};

} // namespace soundsphere

#endif

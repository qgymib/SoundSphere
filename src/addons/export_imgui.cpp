#include <ev.h>
#include <imgui.h>
#include <type_traits>
#include "__init__.hpp"

typedef struct SS_ImVec2_Impl
{
    SS_ImVec2   base;
    ImVec2*     iner;
} SS_ImVec2_Impl_t;
static_assert(std::is_pod<SS_ImVec2_Impl_t>::value);

typedef struct SS_ImVec4_Impl
{
    SS_ImVec4   base;
    ImVec4*     iner;
} SS_ImVec4_Impl_t;
static_assert(std::is_pod<SS_ImVec4_Impl_t>::value);

struct InputTextHelper
{
    SS_ImGuiInputTextCallback   cb;
    void*                       data;
};

int _export_imgui_Begin(const char* name, int* p_open, int flags)
{
    if (p_open == nullptr)
    {
        return ImGui::Begin(name, nullptr, flags);
    }
    
    bool open = *p_open;
    bool ret = ImGui::Begin(name, &open, flags);
    *p_open = open;
    return ret;
}

static void _SS_ImVec2_release(struct SS_ImVec2* thiz)
{
    SS_ImVec2_Impl_t* impl = EV_CONTAINER_OF(thiz, SS_ImVec2_Impl_t, base);
    delete impl->iner;
    delete impl;
}

static SS_ImVec2* _export_imgui_ImVec2(float x, float y)
{
    SS_ImVec2_Impl_t* impl = new SS_ImVec2_Impl_t;
    impl->base.release = _SS_ImVec2_release;
    impl->iner = new ImVec2(x, y);
    return &impl->base;
}

static void _SS_ImVec4_release(struct SS_ImVec4* thiz)
{
    SS_ImVec4_Impl_t* impl = EV_CONTAINER_OF(thiz, SS_ImVec4_Impl_t, base);
    delete impl->iner;
    delete impl;
}

static SS_ImVec4* _export_imgui_ImVec4(float x, float y, float z, float w)
{
    SS_ImVec4_Impl_t* impl = new SS_ImVec4_Impl_t;
    impl->base.release = _SS_ImVec4_release;
    impl->iner = new ImVec4(x, y, z, w);
    return &impl->base;
}

static int _export_BeginChild(const char* str_id, const SS_ImVec2* size,
    int child_flags, int window_flags)
{
    SS_ImVec2_Impl_t* real_sz = EV_CONTAINER_OF(size, SS_ImVec2_Impl_t, base);
    return ImGui::BeginChild(str_id, *real_sz->iner, child_flags, window_flags);
}

static void _export_GetWindowPos(SS_ImVec2* pos)
{
    SS_ImVec2_Impl_t* real_pos = EV_CONTAINER_OF(pos, SS_ImVec2_Impl_t, base);
    *real_pos->iner = ImGui::GetWindowPos();
}

static int _export_IsWindowAppearing(void)
{
    return ImGui::IsWindowAppearing();
}

static int _export_IsWindowCollapsed(void)
{
    return ImGui::IsWindowCollapsed();
}

static void _export_GetWindowSize(SS_ImVec2* size)
{
    SS_ImVec2_Impl_t* real_size = EV_CONTAINER_OF(size, SS_ImVec2_Impl_t, base);
    *real_size->iner = ImGui::GetWindowSize();
}

static void _export_SetNextWindowPos(const SS_ImVec2* pos, int cond, const SS_ImVec2* pivot)
{
    SS_ImVec2_Impl_t* real_pos = EV_CONTAINER_OF(pos, SS_ImVec2_Impl_t, base);

    if (pivot == nullptr)
    {
        ImGui::SetNextWindowPos(*real_pos->iner, cond);
    }
    else
    {
        SS_ImVec2_Impl_t* real_pivot = EV_CONTAINER_OF(pivot, SS_ImVec2_Impl_t, base);
        ImGui::SetNextWindowPos(*real_pos->iner, cond, *real_pivot->iner);
    }
}

static void _export_SetNextWindowSize(const SS_ImVec2* size, int cond)
{
    SS_ImVec2_Impl_t* real_size = EV_CONTAINER_OF(size, SS_ImVec2_Impl_t, base);
    ImGui::SetNextWindowSize(*real_size->iner, cond);
}

static void _export_Dummy(const SS_ImVec2* size)
{
    SS_ImVec2_Impl_t* real_size = EV_CONTAINER_OF(size, SS_ImVec2_Impl_t, base);
    ImGui::Dummy(*real_size->iner);
}

static void _export_TextColored(const SS_ImVec4* col, const char* fmt, ...)
{
    SS_ImVec4_Impl_t* real_col = EV_CONTAINER_OF(col, SS_ImVec4_Impl_t, base);

    va_list ap;
    va_start(ap, fmt);
    ImGui::TextColoredV(*real_col->iner, fmt, ap);
    va_end(ap);
}

static int _export_Button(const char* label, const SS_ImVec2* size)
{
    if (size == nullptr)
    {
        return ImGui::Button(label);
    }

    SS_ImVec2_Impl_t* real_size = EV_CONTAINER_OF(size, SS_ImVec2_Impl_t, base);
    return ImGui::Button(label, *real_size->iner);
}

static int _export_Checkbox(const char* label, int* v)
{
    bool wrap_v = *v;
    bool ret = ImGui::Checkbox(label, &wrap_v);
    *v = wrap_v;
    return ret;
}

static int _export_InputTextCb(ImGuiInputTextCallbackData* data)
{
    InputTextHelper* helper = (InputTextHelper*)data->UserData;

    SS_ImGuiInputTextCallbackData wrap;
    wrap.EventFlag = data->EventFlag;
    wrap.Flags = data->Flags;
    wrap.UserData = helper->data;
    wrap.Buf = data->Buf;
    wrap.BufTextLen = data->BufTextLen;
    wrap.BufSize = data->BufSize;
    wrap.BufDirty = data->BufDirty;
    wrap.CursorPos = data->CursorPos;
    wrap.SelectionStart = data->SelectionStart;
    wrap.SelectionEnd = data->SelectionEnd;

    return helper->cb(&wrap);
}

static int _export_InputText(const char* label, char* buf, unsigned buf_sz, int flags,
        SS_ImGuiInputTextCallback cb, void* udata)
{
    InputTextHelper helper = { cb, udata };
    return ImGui::InputText(label, buf, buf_sz, flags, _export_InputTextCb, &helper);
}

static int _export_InputFloat(const char* label, float* v, float step, float step_fast,
        const char* format, int flags)
{
    return ImGui::InputFloat(label, v, step, step_fast, format, flags);
}

static int _export_InputInt(const char* label, int* v, int step, int step_fast, int flags)
{
    return ImGui::InputInt(label, v, step, step_fast, flags);
}

static int _export_BeginMenuBar(void)
{
    return ImGui::BeginMenuBar();
}

static int _export_BeginMainMenuBar(void)
{
    return ImGui::BeginMainMenuBar();
}

static int _export_BeginMenu(const char* label, int enabled)
{
    return ImGui::BeginMenu(label, enabled);
}

static int _export_MenuItem(const char* label, const char* shortcut,
    int* p_selected, int enabled)
{
    bool selected = *p_selected;
    bool ret = ImGui::MenuItem(label, shortcut, &selected, enabled);
    *p_selected = selected;

    return ret;
}

static int _export_BeginTooltip(void)
{
    return ImGui::BeginTooltip();
}

const SS_ImGui soundsphere::export_com_imgui = {
    _export_imgui_ImVec2,
    _export_imgui_ImVec4,

    _export_imgui_Begin,
    ImGui::End,

    _export_BeginChild,
    ImGui::EndChild,

    _export_IsWindowAppearing,
    _export_IsWindowCollapsed,
    _export_GetWindowPos,
    _export_GetWindowSize,
    ImGui::GetWindowWidth,
    ImGui::GetWindowHeight,

    _export_SetNextWindowPos,
    _export_SetNextWindowSize,

    ImGui::GetScrollX,
    ImGui::GetScrollY,
    ImGui::SetScrollX,
    ImGui::SetScrollY,

     ImGui::PushItemWidth,
     ImGui::PopItemWidth,
     ImGui::SetNextItemWidth,

     ImGui::Separator,
     ImGui::SameLine,
     ImGui::NewLine,
     ImGui::Spacing,
     _export_Dummy,
     ImGui::BeginGroup,
     ImGui::EndGroup,
     ImGui::GetTextLineHeight,
     ImGui::GetFrameHeight,

     ImGui::PushID,
     ImGui::PopID,

     ImGui::Text,
     _export_TextColored,
     ImGui::TextDisabled,
     ImGui::LabelText,
     ImGui::BulletText,
     ImGui::SeparatorText,

     _export_Button,
     _export_Checkbox,
     ImGui::Bullet,
     ImGui::TextLinkOpenURL,

     _export_InputText,
     _export_InputFloat,
     _export_InputInt,

     _export_BeginMenuBar,
     ImGui::EndMenuBar,
     _export_BeginMainMenuBar,
     ImGui::EndMainMenuBar,
     _export_BeginMenu,
     ImGui::EndMenu,
     _export_MenuItem,

     _export_BeginTooltip,
     ImGui::EndTooltip,
     ImGui::SetTooltip,
};

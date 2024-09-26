#ifndef SOUND_SPHERE_EXPORT_IMGUI_H
#define SOUND_SPHERE_EXPORT_IMGUI_H
#ifdef __cplusplus
extern "C" {
#endif

#define SOUNDSPHERE_IMGUIWINDOWFLAGS(xx)                        \
    xx(ImGuiWindowFlags_None,                       0)          \
    xx(ImGuiWindowFlags_NoTitleBar,                 1 << 0)     \
    xx(ImGuiWindowFlags_NoResize,                   1 << 1)     \
    xx(ImGuiWindowFlags_NoMove,                     1 << 2)     \
    xx(ImGuiWindowFlags_NoScrollbar,                1 << 3)     \
    xx(ImGuiWindowFlags_NoScrollWithMouse,          1 << 4)     \
    xx(ImGuiWindowFlags_NoCollapse,                 1 << 5)     \
    xx(ImGuiWindowFlags_AlwaysAutoResize,           1 << 6)     \
    xx(ImGuiWindowFlags_NoBackground,               1 << 7)     \
    xx(ImGuiWindowFlags_NoSavedSettings,            1 << 8)     \
    xx(ImGuiWindowFlags_NoMouseInputs,              1 << 9)     \
    xx(ImGuiWindowFlags_MenuBar,                    1 << 10)    \
    xx(ImGuiWindowFlags_HorizontalScrollbar,        1 << 11)    \
    xx(ImGuiWindowFlags_NoFocusOnAppearing,         1 << 12)    \
    xx(ImGuiWindowFlags_NoBringToFrontOnFocus,      1 << 13)    \
    xx(ImGuiWindowFlags_AlwaysVerticalScrollbar,    1 << 14)    \
    xx(ImGuiWindowFlags_AlwaysHorizontalScrollbar,  1 << 15)    \
    xx(ImGuiWindowFlags_NoNavInputs,                1 << 16)    \
    xx(ImGuiWindowFlags_NoNavFocus,                 1 << 17)    \
    xx(ImGuiWindowFlags_UnsavedDocument,            1 << 18)

#define SOUNDSPHERE_IMGUICHILDFLAGS(xx)                 \
    xx(ImGuiChildFlags_None,                    0)      \
    xx(ImGuiChildFlags_Borders,                 1 << 0) \
    xx(ImGuiChildFlags_AlwaysUseWindowPadding,  1 << 1) \
    xx(ImGuiChildFlags_ResizeX,                 1 << 2) \
    xx(ImGuiChildFlags_ResizeY,                 1 << 3) \
    xx(ImGuiChildFlags_AutoResizeX,             1 << 4) \
    xx(ImGuiChildFlags_AutoResizeY,             1 << 5) \
    xx(ImGuiChildFlags_AlwaysAutoResize,        1 << 6) \
    xx(ImGuiChildFlags_FrameStyle,              1 << 7) \
    xx(ImGuiChildFlags_NavFlattened,            1 << 8)

#define SOUNDSPHERE_IMGUICOND(xx)       \
    xx(ImGuiCond_None,          0)      \
    xx(ImGuiCond_Always,        1 << 0) \
    xx(ImGuiCond_Once,          1 << 1) \
    xx(ImGuiCond_FirstUseEver,  1 << 2) \
    xx(ImGuiCond_Appearing,     1 << 3)

#define SOUNDSPHERE_IMGUIINPUTTEXTFLAGS(xx)                 \
    xx(ImGuiInputTextFlags_None,                0)          \
    xx(ImGuiInputTextFlags_CharsDecimal,        1 << 0)     \
    xx(ImGuiInputTextFlags_CharsHexadecimal,    1 << 1)     \
    xx(ImGuiInputTextFlags_CharsScientific,     1 << 2)     \
    xx(ImGuiInputTextFlags_CharsUppercase,      1 << 3)     \
    xx(ImGuiInputTextFlags_CharsNoBlank,        1 << 4)     \
    xx(ImGuiInputTextFlags_AllowTabInput,       1 << 5)     \
    xx(ImGuiInputTextFlags_EnterReturnsTrue,    1 << 6)     \
    xx(ImGuiInputTextFlags_EscapeClearsAll,     1 << 7)     \
    xx(ImGuiInputTextFlags_CtrlEnterForNewLine, 1 << 8)     \
    xx(ImGuiInputTextFlags_ReadOnly,            1 << 9)     \
    xx(ImGuiInputTextFlags_Password,            1 << 10)    \
    xx(ImGuiInputTextFlags_AlwaysOverwrite,     1 << 11)    \
    xx(ImGuiInputTextFlags_AutoSelectAll,       1 << 12)    \
    xx(ImGuiInputTextFlags_ParseEmptyRefVal,    1 << 13)    \
    xx(ImGuiInputTextFlags_DisplayEmptyRefVal,  1 << 14)    \
    xx(ImGuiInputTextFlags_NoHorizontalScroll,  1 << 15)    \
    xx(ImGuiInputTextFlags_NoUndoRedo,          1 << 16)    \
    xx(ImGuiInputTextFlags_CallbackCompletion,  1 << 17)    \
    xx(ImGuiInputTextFlags_CallbackHistory,     1 << 18)    \
    xx(ImGuiInputTextFlags_CallbackAlways,      1 << 19)    \
    xx(ImGuiInputTextFlags_CallbackCharFilter,  1 << 20)    \
    xx(ImGuiInputTextFlags_CallbackResize,      1 << 21)    \
    xx(ImGuiInputTextFlags_CallbackEdit,        1 << 22)

enum SS_ImGuiWindowFlags_
{
#define SOUNDSPHERE_EXPAND_IMGUIWINDOWFLAGS(a, b)    SS_##a = b,
    SOUNDSPHERE_IMGUIWINDOWFLAGS(SOUNDSPHERE_EXPAND_IMGUIWINDOWFLAGS)
#undef SOUNDSPHERE_EXPAND_IMGUIWINDOWFLAGS

    SS_ImGuiWindowFlags_NoNav          = SS_ImGuiWindowFlags_NoNavInputs | SS_ImGuiWindowFlags_NoNavFocus,
    SS_ImGuiWindowFlags_NoDecoration   = SS_ImGuiWindowFlags_NoTitleBar | SS_ImGuiWindowFlags_NoResize | SS_ImGuiWindowFlags_NoScrollbar | SS_ImGuiWindowFlags_NoCollapse,
    SS_ImGuiWindowFlags_NoInputs       = SS_ImGuiWindowFlags_NoMouseInputs | SS_ImGuiWindowFlags_NoNavInputs | SS_ImGuiWindowFlags_NoNavFocus,
};

enum SS_ImGuiChildFlags_
{
#define SOUNDSPHERE_EXPAND_IMGUICHILDFLAGS(a, b)    SS_##a = b,
    SOUNDSPHERE_IMGUICHILDFLAGS(SOUNDSPHERE_EXPAND_IMGUICHILDFLAGS)
#undef SOUNDSPHERE_EXPAND_IMGUICHILDFLAGS
};

enum SS_ImGuiCond_
{
#define SOUNDSPHERE_EXPAND_IMGUICOND(a, b)  SS_##a = b,
    SOUNDSPHERE_IMGUICOND(SOUNDSPHERE_EXPAND_IMGUICOND)
#undef SOUNDSPHERE_EXPAND_IMGUICOND
};

enum SS_ImGuiInputTextFlags_
{
#define SOUNDSPHERE_EXPAND_IMGUIINPUTTEXTFLAGS(a, b)    SS_##a = b,
    SOUNDSPHERE_IMGUIINPUTTEXTFLAGS(SOUNDSPHERE_EXPAND_IMGUIINPUTTEXTFLAGS)
#undef SOUNDSPHERE_EXPAND_IMGUIINPUTTEXTFLAGS
};

typedef struct SS_ImVec2
{
    void (*release)(struct SS_ImVec2* thiz);
} SS_ImVec2;

typedef struct SS_ImVec4
{
    void (*release)(struct SS_ImVec4* thiz);
} SS_ImVec4;

typedef struct SS_ImGuiInputTextCallbackData
{
    int     EventFlag;
    int     Flags;
    void*   UserData;

    char*   Buf;
    int     BufTextLen;
    int     BufSize;
    int     BufDirty;
    int     CursorPos;
    int     SelectionStart;
    int     SelectionEnd;
} SS_ImGuiInputTextCallbackData;

typedef int (*SS_ImGuiInputTextCallback)(SS_ImGuiInputTextCallbackData* data);

typedef struct SS_ImGui
{
    SS_ImVec2* (*ImVec2)(float x, float y);
    SS_ImVec4* (*ImVec4)(float x, float y, float z, float w);

    int (*Begin)(const char* name, int* p_open, int flags);
    void (*End)(void);

    int (*BeginChild)(const char* str_id, const SS_ImVec2* size, int child_flags, int window_flags);
    void (*EndChild)(void);

    int (*IsWindowAppearing)(void);
    int (*IsWindowCollapsed)(void);
    void (*GetWindowPos)(SS_ImVec2* pos);
    void (*GetWindowSize)(SS_ImVec2* size);
    float (*GetWindowWidth)(void);
    float (*GetWindowHeight)(void);

    void (*SetNextWindowPos)(const SS_ImVec2* pos, int cond, const SS_ImVec2* pivot);
    void (*SetNextWindowSize)(const SS_ImVec2* size, int cond);

    float (*GetScrollX)(void);
    float (*GetScrollY)(void);
    void (*SetScrollX)(float x);
    void (*SetScrollY)(float y);

    void (*PushItemWidth)(float width);
    void (*PopItemWidth)(void);
    void (*SetNextItemWidth)(float width);

    void (*Separator)(void);
    void (*SameLine)(float offset, float spacing);
    void (*NewLine)(void);
    void (*Spacing)(void);
    void (*Dummy)(const SS_ImVec2* size);
    void (*BeginGroup)(void);
    void (*EndGroup)(void);
    float (*GetTextLineHeight)(void);
    float (*GetFrameHeight)(void);

    void (*PushID)(const char* str_id);
    void (*PopID)(void);

    void (*Text)(const char* fmt, ...);
    void (*TextColored)(const SS_ImVec4* col, const char* fmt, ...);
    void (*TextDisabled)(const char* fmt, ...);
    void (*LabelText)(const char* label, const char* fmt, ...);
    void (*BulletText)(const char* fmt, ...);
    void (*SeparatorText)(const char* label);

    int (*Button)(const char* label, const SS_ImVec2* size);
    int (*Checkbox)(const char* label, int* v);
    void (*Bullet)(void);
    void (*TextLinkOpenURL)(const char* label, const char* url);

    int (*InputText)(const char* label, char* buf, unsigned buf_sz, int flags,
        SS_ImGuiInputTextCallback cb, void* udata);
    int (*InputFloat)(const char* label, float* v, float step, float step_fast,
        const char* format, int flags);
    int (*InputInt)(const char* label, int* v, int step, int step_fast, int flags);

    int (*BeginMenuBar)(void);
    void (*EndMenuBar)(void);
    int (*BeginMainMenuBar)(void);
    void (*EndMainMenuBar)(void);
    int (*BeginMenu)(const char* label, int enabled);
    void (*EndMenu)(void);
    int (*MenuItem)(const char* label, const char* shortcut, int* p_selected, int enabled);

    int (*BeginTooltip)(void);
    void (*EndTooltip)(void);
    void (*SetTooltip)(const char* fmt, ...);
} SS_ImGui;

#ifdef __cplusplus
}
#endif
#endif

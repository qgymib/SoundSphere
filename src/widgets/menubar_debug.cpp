#include "i18n/__init__.h"
#include "__init__.hpp"

typedef struct debug_ctx
{
    debug_ctx();

    /**
     * @brief Show debug window.
     */
    bool show_window;

    /**
     * @brief Show imgui demo window.
     */
    bool show_imgui_demo;
} debug_ctx_t;

static debug_ctx_t *s_debug_ctx = nullptr;

debug_ctx::debug_ctx()
{
    show_window = false;
    show_imgui_demo = false;
}

static void _menubar_debug_init(void)
{
    s_debug_ctx = new debug_ctx_t;
}

static void _menubar_debug_exit(void)
{
    delete s_debug_ctx;
    s_debug_ctx = nullptr;
}

static void _menubar_debug_draw(void)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(_T->help))
        {
            ImGui::MenuItem(_T->debug, nullptr, &s_debug_ctx->show_window);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if (!s_debug_ctx->show_window)
    {
        return;
    }

    const char *window_title = _T->debug;
    if (ImGui::Begin(window_title, &s_debug_ctx->show_window))
    {
        ImGui::Checkbox("Show Imgui Demo Window", &s_debug_ctx->show_imgui_demo);
        if (s_debug_ctx->show_imgui_demo)
        {
            ImGui::ShowDemoWindow(&s_debug_ctx->show_imgui_demo);
        }
    }
    ImGui::End();
}

const soundsphere::widget_t soundsphere::menubar_debug = {
    _menubar_debug_init,
    _menubar_debug_exit,
    _menubar_debug_draw,
};

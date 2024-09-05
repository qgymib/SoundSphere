#include <imgui.h>
#include <ev.h>
#include "version.hpp"
#include "i18n/__init__.hpp"
#include "utils/defines.hpp"
#include "__init__.hpp"

typedef struct dep_info
{
    const char* name;
    const char* version;
    const char* url;
} dep_info_t;

static bool s_menu_about_show = false;

static void _widget_about_init(void)
{
}

static void _widget_about_exit(void)
{
}

static void _widget_about_show_config_frame(void)
{
    static dep_info_t third_party_list[] = {
        { "imgui", IMGUI_VERSION, "https://github.com/ocornut/imgui" },
        { "IconFontCppHeaders", "", "https://github.com/juliettef/IconFontCppHeaders" },
        { "libev", ev_version_str(), "https://github.com/qgymib/libev" },
    };

    ImVec2 child_sz = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 4);
    if (ImGui::BeginChild("about_show_config_info", child_sz, ImGuiChildFlags_FrameStyle))
    {
        for (size_t i = 0; i < IM_ARRAYSIZE(third_party_list); i++)
        {
            dep_info_t* info = &third_party_list[i];
            ImGui::Text("%s: %s", info->name, info->version);
            ImGui::SameLine();
            ImGui::TextLinkOpenURL(soundsphere::_i18n->homepage, info->url);
        }
    }
    ImGui::EndChild();
}

static void _widget_about_draw_menu(void)
{
    const char* window_title = soundsphere::_i18n->about;
    if (!ImGui::Begin(window_title, &s_menu_about_show, ImGuiWindowFlags_AlwaysAutoResize))
    {
        goto finish;
    }

    ImGui::Text(PROG_NAME " %s", soundsphere::version());
    ImGui::TextLinkOpenURL(soundsphere::_i18n->homepage, "https://github.com/qgymib/SoundSphere");
    ImGui::Separator();

    static bool show_imgui_demo_window = false;
    ImGui::Checkbox("Show Imgui Demo Window", &show_imgui_demo_window);
    if (show_imgui_demo_window)
    {
        ImGui::ShowDemoWindow(&show_imgui_demo_window);
    }

    static bool show_config_info = false;
    ImGui::Checkbox(soundsphere::_i18n->about_show_config_info, &show_config_info);
    if (show_config_info)
    {
        _widget_about_show_config_frame();
    }

finish:
    ImGui::End();
}

static void _widget_about_draw(void)
{
    /* Register to MainMenu. */
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(soundsphere::_i18n->help))
        {
            ImGui::MenuItem(soundsphere::_i18n->about, nullptr, &s_menu_about_show);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    /* Show window. */
    if (s_menu_about_show)
    {
        _widget_about_draw_menu();
    }
}

const soundsphere::widget_t soundsphere::menubar_about = {
_widget_about_init,
_widget_about_exit,
_widget_about_draw,
};

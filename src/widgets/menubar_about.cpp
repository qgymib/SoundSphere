#include <imgui.h>
#include <ev.h>
#include <taglib/taglib.h>
#include <spdlog/spdlog.h>
#include <SDL2/SDL_mixer.h>
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

#define TAGLIB_VERSION  \
    STRINGIFY(TAGLIB_MAJOR_VERSION) "." STRINGIFY(TAGLIB_MINOR_VERSION) "." STRINGIFY(TAGLIB_PATCH_VERSION)

#undef SPDLOG_VERSION
#define SPDLOG_VERSION  \
    STRINGIFY(SPDLOG_VER_MAJOR) "." STRINGIFY(SPDLOG_VER_MINOR) "." STRINGIFY(SPDLOG_VER_PATCH)

#undef SDL_MIXER_VERSION
#define SDL_MIXER_VERSION   \
    STRINGIFY(SDL_MIXER_MAJOR_VERSION) "." STRINGIFY(SDL_MIXER_MINOR_VERSION) "." STRINGIFY(SDL_MIXER_PATCHLEVEL)

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
        { "libev", ev_version_str(), "https://github.com/qgymib/libev" },
        { "spdlog", SPDLOG_VERSION, "https://github.com/gabime/spdlog" },
        { "stb", "", "https://github.com/nothings/stb" },
        { "IconFontCppHeaders", "", "https://github.com/juliettef/IconFontCppHeaders" },
        { "SDL_mixer", SDL_MIXER_VERSION, "https://github.com/libsdl-org/SDL_mixer" },
        { "TagLib", TAGLIB_VERSION, "https://taglib.org/" },
    };

    ImVec2 child_sz = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 8);
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

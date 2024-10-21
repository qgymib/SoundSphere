#include <ev.h>
#include <base64.h>
#include <imgui.h>
#include <taglib/taglib.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <SDL_mixer.h>
#include <zlib.h>
#include "i18n/__init__.h"
#include "utils/defines.hpp"
#include "__init__.hpp"

#if defined(IMGUI_ENABLE_FREETYPE)
#include <freetype/freetype.h>
#undef FREETYPE_VERSION
#define FREETYPE_VERSION STRINGIFY(FREETYPE_MAJOR) "." STRINGIFY(FREETYPE_MINOR) "." STRINGIFY(FREETYPE_PATCH)
#endif

#define TAGLIB_VERSION                                                                                                 \
    STRINGIFY(TAGLIB_MAJOR_VERSION) "." STRINGIFY(TAGLIB_MINOR_VERSION) "." STRINGIFY(TAGLIB_PATCH_VERSION)

#undef SPDLOG_VERSION
#define SPDLOG_VERSION STRINGIFY(SPDLOG_VER_MAJOR) "." STRINGIFY(SPDLOG_VER_MINOR) "." STRINGIFY(SPDLOG_VER_PATCH)

#undef SDL_MIXER_VERSION
#define SDL_MIXER_VERSION                                                                                              \
    STRINGIFY(SDL_MIXER_MAJOR_VERSION) "." STRINGIFY(SDL_MIXER_MINOR_VERSION) "." STRINGIFY(SDL_MIXER_PATCHLEVEL)

#undef JSON_VERSION
#define JSON_VERSION                                                                                                   \
    STRINGIFY(NLOHMANN_JSON_VERSION_MAJOR)                                                                             \
    "." STRINGIFY(NLOHMANN_JSON_VERSION_MINOR) "." STRINGIFY(NLOHMANN_JSON_VERSION_PATCH)

typedef struct dep_info
{
    const char *name;
    const char *version;
    const char *url;
} dep_info_t;

static bool s_menu_about_show = false;

static void _widget_about_init(void)
{
}

static void _widget_about_exit(void)
{
}

static void _widget_about_3rd(void)
{
    static dep_info_t third_party_list[] = {
        { "cpp-base64",         "",                "https://github.com/ReneNyffenegger/cpp-base64"   },
#if defined(IMGUI_ENABLE_FREETYPE)
        { "freetype",           FREETYPE_VERSION,  "https://freetype.org"                            },
#endif
        { "imgui",              IMGUI_VERSION,     "https://github.com/ocornut/imgui"                },
        { "libev",              ev_version_str(),  "https://github.com/qgymib/libev"                 },
        { "nlohmann-json",      JSON_VERSION,      "https://json.nlohmann.me"                        },
        { "spdlog",             SPDLOG_VERSION,    "https://github.com/gabime/spdlog"                },
        { "stb",                "",                "https://github.com/nothings/stb"                 },
        { "zlib",               ZLIB_VERSION,      "https://www.zlib.net"                            },
        { "IconFontCppHeaders", "",                "https://github.com/juliettef/IconFontCppHeaders" },
        { "SDL_mixer",          SDL_MIXER_VERSION, "https://github.com/libsdl-org/SDL_mixer"         },
        { "TagLib",             TAGLIB_VERSION,    "https://taglib.org/"                             },
    };

    ImGui::SeparatorText("3rd");

    const int table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable("about_3rd", 3, table_flags))
    {
        ImGui::TableSetupColumn(_T->name);
        ImGui::TableSetupColumn(_T->version);
        ImGui::TableSetupColumn(_T->homepage);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(IM_ARRAYSIZE(third_party_list));

        while (clipper.Step())
        {
            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
            {
                dep_info_t *info = &third_party_list[row_n];
                ImGui::PushID((void *)info);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", info->name);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", info->version);

                ImGui::TableSetColumnIndex(2);
                ImGui::TextLinkOpenURL(_T->homepage, info->url);

                ImGui::PopID();
            }
        }

        ImGui::EndTable();
    }
}

static void _widget_about_show_config_frame(void)
{
    ImVec2 child_sz = ImVec2(320, ImGui::GetTextLineHeightWithSpacing() * 8);
    if (ImGui::BeginChild("about_show_config_info", child_sz, ImGuiChildFlags_FrameStyle))
    {
        _widget_about_3rd();
    }
    ImGui::EndChild();
}

static void _widget_about_draw_menu(void)
{
    const char *window_title = _T->about;
    if (!ImGui::Begin(window_title, &s_menu_about_show, ImGuiWindowFlags_AlwaysAutoResize))
    {
        goto finish;
    }

    ImGui::Text(PROJECT_NAME " %s", PROJECT_VERSION);
    ImGui::TextLinkOpenURL(_T->homepage, "https://github.com/qgymib/SoundSphere");
    ImGui::Separator();

    static bool show_config_info = false;
    ImGui::Checkbox(_T->about_show_config_info, &show_config_info);
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
        if (ImGui::BeginMenu(_T->help))
        {
            ImGui::MenuItem(_T->about, nullptr, &s_menu_about_show);
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
    nullptr,
};

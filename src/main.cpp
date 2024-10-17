// Dear ImGui: standalone example application for SDL2 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context
// creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important to understand: SDL_Renderer is an _optional_ component of SDL2.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.

#include <imgui.h>
#include <stdio.h>
#include <IconsFontAwesome6.h>
#include <curl/curl.h>
#include "backends/__init__.hpp"
#include "config/__init__.hpp"
#include "fonts/fa_solid_900.h"
#include "fonts/NotoSans.h"
#include "fonts/NotoSansKR.h"
#include "fonts/NotoSansSC.h"
#include "i18n/__init__.h"
#include "runtime/__init__.hpp"
#include "utils/defines.hpp"
#include "widgets/__init__.hpp"

typedef struct soundsphere_module
{
    void (*init)(void);
    void (*exit)(void);
} soundsphere_module_t;

static void _curl_init(void)
{
    curl_global_init(CURL_GLOBAL_ALL);
}

/**
 * @brief Registered modules.
 * Modules are initialized in order and cleanup in reverse order.
 */
static soundsphere_module_t s_modules[] = {
    { soundsphere::config_init,  soundsphere::config_exit  },
    { soundsphere_i18n_init,     soundsphere_i18n_exit     },
    { soundsphere::runtime_init, soundsphere::runtime_exit },
    { soundsphere::widget_init,  soundsphere::widget_exit  },
    { _curl_init,                curl_global_cleanup       },
};

// Main code
int main(int, char **)
{
    soundsphere::backend_init();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.IniFilename = nullptr;

    /* Setup Dear ImGui style. */
    ImGui::StyleColorsDark();

    io.Fonts->AddFontFromMemoryCompressedTTF(NotoSans_compressed_data, NotoSans_compressed_size, 18.0f, nullptr,
                                             io.Fonts->GetGlyphRangesDefault());
    {
        ImFontConfig config;
        config.MergeMode = true;
        io.Fonts->AddFontFromMemoryCompressedTTF(NotoSansSC_compressed_data, NotoSansSC_compressed_size, 18.0f, &config,
                                                 io.Fonts->GetGlyphRangesChineseFull());
    }
    {
        ImFontConfig config;
        config.MergeMode = true;
        io.Fonts->AddFontFromMemoryCompressedTTF(NotoSansKR_compressed_data, NotoSansKR_compressed_size, 18.0f, &config,
                                                 io.Fonts->GetGlyphRangesKorean());
    }
    {
        ImFontConfig config;
        config.MergeMode = true;
        config.GlyphMinAdvanceX = 18.0f;
        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        io.Fonts->AddFontFromMemoryCompressedTTF(fa_solid_900_compressed_data, fa_solid_900_compressed_size,
                                                 config.GlyphMinAdvanceX, &config, icon_ranges);
    }
    io.Fonts->Build();

    for (size_t i = 0; i < ARRAY_SIZE(s_modules); i++)
    {
        s_modules[i].init();
    }

    /* Main loop. */
    soundsphere::backend_draw([]() {
        soundsphere::runtime_loop();
        soundsphere::widget_draw();
    });

    for (size_t i = ARRAY_SIZE(s_modules); i > 0; i--)
    {
        s_modules[i - 1].exit();
    }

    soundsphere::backend_exit();

    return 0;
}

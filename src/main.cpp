// Dear ImGui: standalone example application for SDL2 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

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
#include "backends/__init__.hpp"
#include "fonts/fa_solid_900.h"
#include "fonts/NotoSans.h"
#include "fonts/NotoSansKR.h"
#include "fonts/NotoSansSC.h"
#include "i18n/__init__.hpp"
#include "runtime/__init__.hpp"
#include "widgets/__init__.hpp"

// Main code
int main(int, char**)
{
    soundsphere::backend_init();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.IniFilename = nullptr;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    io.Fonts->AddFontFromMemoryCompressedTTF(NotoSans_compressed_data, NotoSans_compressed_size,
        18.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
    {
        ImFontConfig config;
        config.MergeMode = true;
        io.Fonts->AddFontFromMemoryCompressedTTF(NotoSansSC_compressed_data, NotoSansSC_compressed_size,
            18.0f, &config, io.Fonts->GetGlyphRangesChineseFull());
    }
    {
        ImFontConfig config;
        config.MergeMode = true;
        io.Fonts->AddFontFromMemoryCompressedTTF(NotoSansKR_compressed_data, NotoSansKR_compressed_size,
            18.0f, &config, io.Fonts->GetGlyphRangesKorean());
    }
    {
        ImFontConfig config;
        config.MergeMode = true;
        config.GlyphMinAdvanceX = 18.0f;
        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0};
        io.Fonts->AddFontFromMemoryCompressedTTF(fa_solid_900_compressed_data, fa_solid_900_compressed_size,
            config.GlyphMinAdvanceX, &config, icon_ranges);
    }
    io.Fonts->Build();

    soundsphere::i18n_init();
    soundsphere::runtime_init();
    soundsphere::widget_init();

    /* Main loop. */
    soundsphere::backend_draw([](){
        soundsphere::runtime_loop();
        soundsphere::widget_draw();
    });

    soundsphere::widget_exit();
    soundsphere::runtime_exit();
    soundsphere::i18n_exit();

    soundsphere::backend_exit();

    return 0;
}
#include <ev.h>
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include "utils/defines.hpp"
#include "__init__.hpp"

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

static SDL_Renderer *s_renderer = nullptr;
static SDL_Window *s_window = nullptr;

void soundsphere::backend_init(void)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        spdlog::error("Error: {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    s_window = SDL_CreateWindow(PROJECT_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (s_window == nullptr)
    {
        spdlog::error("Error: SDL_CreateWindow(): {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    s_renderer = SDL_CreateRenderer(s_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (s_renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        exit(EXIT_FAILURE);
    }

    // SDL_RendererInfo info;
    // SDL_GetRendererInfo(renderer, &info);
    // SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(s_window, s_renderer);
    ImGui_ImplSDLRenderer2_Init(s_renderer);
}

void soundsphere::backend_exit(void)
{
    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(s_renderer);
    SDL_DestroyWindow(s_window);
    SDL_Quit();

    s_renderer = nullptr;
    s_window = nullptr;
}

void soundsphere::backend_draw(soundsphere::DrawFn fn)
{
    // Our state
    ImGuiIO &io = ImGui::GetIO();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your
        // inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or
        // clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or
        // clear/overwrite your copy of the keyboard data. Generally you may always pass all inputs to dear imgui, and
        // hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(s_window))
                done = true;
        }
        if (SDL_GetWindowFlags(s_window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        fn();

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(s_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(s_renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255),
                               (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(s_renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), s_renderer);
        SDL_RenderPresent(s_renderer);
    }
}

soundsphere::Texture soundsphere::backend_load_image(const void *data, size_t size)
{
    int channels = 0;
    int img_width = 0;
    int img_height = 0;

    unsigned char *img_data = stbi_load_from_memory((stbi_uc *)data, (int)size, &img_width, &img_height, &channels, 0);
    if (img_data == nullptr)
    {
        return soundsphere::Texture();
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(img_data, img_width, img_height, channels * 8, channels * img_width,
                                                    0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    if (surface == nullptr)
    {
        stbi_image_free(img_data);
        return soundsphere::Texture();
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(s_renderer, surface);
    SDL_FreeSurface(surface);
    stbi_image_free(img_data);

    if (texture == nullptr)
    {
        return soundsphere::Texture();
    }

    return soundsphere::Texture(texture, [](SDL_Texture *t) { SDL_DestroyTexture(t); });
}

soundsphere::Texture soundsphere::backend_load_image_from_file(const char *path)
{
    ev_fs_req_t req;
    ev_fs_readfile(NULL, &req, path, NULL);

    ev_buf_t *buf = ev_fs_get_filecontent(&req);
    soundsphere::Texture ret = backend_load_image(buf->data, buf->size);
    ev_fs_req_cleanup(&req);

    return ret;
}

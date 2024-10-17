set(IMGUI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)

add_library(imgui
    ${IMGUI_ROOT}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_ROOT}/backends/imgui_impl_sdlrenderer2.cpp
    ${IMGUI_ROOT}/misc/cpp/imgui_stdlib.cpp
    ${IMGUI_ROOT}/imgui.cpp
    ${IMGUI_ROOT}/imgui_demo.cpp
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_tables.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
)

target_include_directories(imgui
    PUBLIC
        $<BUILD_INTERFACE:${IMGUI_ROOT}>
        $<BUILD_INTERFACE:${IMGUI_ROOT}/backends>
        $<BUILD_INTERFACE:${IMGUI_ROOT}/misc/cpp>
)

###############################################################################
# Require SDL2
###############################################################################
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(imgui PUBLIC SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
if (TARGET SDL2::SDL2-static)
    target_link_libraries(imgui PUBLIC SDL2::SDL2-static)
else()
    target_link_libraries(imgui PUBLIC SDL2::SDL2)
    if (WIN32)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                 $<TARGET_FILE:SDL2::SDL2>
                 $<TARGET_FILE_DIR:${PROJECT_NAME}>
            VERBATIM
        )
    endif()
endif()

###############################################################################
# Require OpenGL
###############################################################################
#find_package(OpenGL REQUIRED)

#target_include_directories(imgui PUBLIC ${OPENGL_INCLUDE_DIRS})
#target_link_libraries(imgui PUBLIC ${OPENGL_LIBRARIES})

# This is the helper tool for embed fonts
add_executable(binary_to_compressed_c
    ${IMGUI_ROOT}/misc/fonts/binary_to_compressed_c.cpp
)

###############################################################################
# Require Freetype
###############################################################################
find_package(Freetype QUIET)
if (FREETYPE_FOUND)
    target_sources(imgui
        PRIVATE
            ${IMGUI_ROOT}/misc/freetype/imgui_freetype.cpp
    )
    target_compile_options(imgui
        PUBLIC
            -DIMGUI_ENABLE_FREETYPE
    )
    target_link_libraries(imgui
        PUBLIC
            Freetype::Freetype
    )
endif()

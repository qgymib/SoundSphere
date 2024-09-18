cmake_minimum_required(VERSION 3.5)
project(soundsphere)

###############################################################################
# Functions
###############################################################################

# Enable all reasonable warnings and make all warnings into errors.
function(setup_target_wall name)
    if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${name} PRIVATE /W4 /WX)
    else ()
        target_compile_options(${name} PRIVATE -Wall -Wextra -Werror)
    endif ()
endfunction()

###############################################################################
# executable
###############################################################################

set(soundsphere_sources
	"src/assets/icon.c"
    "src/backends/sdl2_sdlrenderer2.cpp"
    "src/fonts/fa_solid_900.c"
    "src/fonts/NotoSans.c"
    "src/fonts/NotoSansKR.c"
    "src/fonts/NotoSansSC.c"
    "src/i18n/__init__.cpp"
    "src/i18n/en_US.c"
    "src/i18n/zh_CN.c"
    "src/runtime/__init__.cpp"
    "src/utils/binary.cpp"
    "src/utils/explorer.cpp"
    "src/utils/imgui.cpp"
    "src/utils/music_tag.cpp"
    "src/utils/path.cpp"
    "src/utils/string.cpp"
    "src/utils/time.cpp"
    "src/widgets/__init__.cpp"
    "src/widgets/dummy_player.cpp"
    "src/widgets/menubar_about.cpp"
    "src/widgets/menubar_open.cpp"
    "src/widgets/menubar_preferences.cpp"
    "src/widgets/menubar_translations.cpp"
    "src/widgets/tool_tag_editor.cpp"
    "src/widgets/ui_cover.cpp"
    "src/widgets/ui_filter.cpp"
    "src/widgets/ui_lyric.cpp"
    "src/widgets/ui_playbar.cpp"
    "src/widgets/ui_playlist.cpp"
    "src/widgets/ui_statusbar.cpp"
    "src/main.cpp"
    "src/version.cpp"
)

if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${soundsphere_sources})
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            /utf-8
            /std:c++17
    )
    target_link_options(${PROJECT_NAME}
        PRIVATE
            /subsystem:windows /entry:mainCRTStartup
    )
else()
    add_executable(${PROJECT_NAME} ${soundsphere_sources})
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

setup_target_wall(${PROJECT_NAME})

###############################################################################
# Dependency
###############################################################################
include(third_party/imgui.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

include(third_party/IconFontCppHeaders.cmake)
target_include_directories(${PROJECT_NAME} PRIVATE ${ICON_FONT_CPP_INCLUDE_DIRS})

include(third_party/libev.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE ev)

include(third_party/taglib.cmake)
target_include_directories(${PROJECT_NAME} PRIVATE ${TAGLIB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${TAGLIB_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${TAGLIB_COMPILE_OPTIONS})

include(third_party/spdlog.cmake)
target_include_directories(${PROJECT_NAME} PRIVATE ${SPDLOG_INCLUDE_DIRS})

find_package(SDL2_mixer REQUIRED)
if (TARGET SDL2_mixer::SDL2_mixer-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer-static)
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer)
endif ()

include(third_party/stb.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE stb)

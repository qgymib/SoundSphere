cmake_minimum_required(VERSION 3.5)
project(soundsphere
    VERSION 1.0.0
    LANGUAGES C CXX
)

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
    "src/config/__init__.cpp"
    "src/fonts/fa_solid_900.c"
    "src/fonts/NotoSans.c"
    "src/fonts/NotoSansKR.c"
    "src/fonts/NotoSansSC.c"
    "src/i18n/__init__.cpp"
    "src/i18n/en_US.c"
    "src/i18n/zh_CN.c"
    "src/runtime/__init__.cpp"
    "src/utils/binary.cpp"
    "src/utils/curl.cpp"
    "src/utils/env.cpp"
    "src/utils/explorer.cpp"
    "src/utils/imgui.cpp"
    "src/utils/krc.cpp"
    "src/utils/music_tag.cpp"
    "src/utils/path.cpp"
    "src/utils/string.cpp"
    "src/utils/time.cpp"
    "src/widgets/__init__.cpp"
    "src/widgets/dummy_player.cpp"
    "src/widgets/menubar_about.cpp"
    "src/widgets/menubar_debug.cpp"
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
    "src/widgets/ui_title.cpp"
    "src/main.cpp"
)

if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${soundsphere_sources} "resource.rc")
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            /utf-8
    )
    target_link_options(${PROJECT_NAME}
        PRIVATE
            /subsystem:windows /entry:mainCRTStartup
    )
else()
    add_executable(${PROJECT_NAME} ${soundsphere_sources})
endif()

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -DCMAKE_PROJECT_NAME=${CMAKE_PROJECT_NAME}
        -DCMAKE_PROJECT_VERSION=${CMAKE_PROJECT_VERSION}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_features(${PROJECT_NAME}
    PRIVATE
        cxx_std_17
)

setup_target_wall(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
)

###############################################################################
# Dependency
###############################################################################

include(third_party/imgui.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

include(third_party/IconFontCppHeaders.cmake)
target_include_directories(${PROJECT_NAME} PRIVATE ${ICON_FONT_CPP_INCLUDE_DIRS})

include(third_party/libev.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE ev)

find_package(taglib REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE TagLib::TagLib)

include(third_party/spdlog.cmake)
target_include_directories(${PROJECT_NAME} PRIVATE ${SPDLOG_INCLUDE_DIRS})

find_package(SDL2_mixer REQUIRED)
if (TARGET SDL2_mixer::SDL2_mixer-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer-static)
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer)
    if (WIN32)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL2_mixer::SDL2_mixer>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            VERBATIM
        )
    endif()
endif ()

include(third_party/stb.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE stb)

include(third_party/json.cmake)
target_include_directories(${PROJECT_NAME} PRIVATE ${JSON_INCLUDE_DIRS})

find_package(CURL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

include(third_party/cpp-base64.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE cpp_base64)

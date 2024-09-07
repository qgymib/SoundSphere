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
    "src/fonts/fa_solid_900.c"
    "src/fonts/NotoSansSC.c"
    "src/i18n/__init__.cpp"
    "src/i18n/en_us.cpp"
    "src/i18n/zh_cn.cpp"
    "src/runtime/__init__.cpp"
    "src/runtime/playitem.cpp"
    "src/utils/explorer.cpp"
    "src/utils/path.cpp"
    "src/utils/string.cpp"
    "src/utils/time.cpp"
    "src/widgets/__init__.cpp"
    "src/widgets/menubar_about.cpp"
    "src/widgets/menubar_open.cpp"
    "src/widgets/menubar_preferences.cpp"
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
            /std:c++20
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
target_link_libraries(${PROJECT_NAME} PRIVATE IconFontCppHeaders)

include(third_party/libev.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE ev)

include(third_party/taglib.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE TagLib)

include(third_party/spdlog.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)

include(third_party/SDL_mixer.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL_mixer)

include(third_party/stb.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE stb)

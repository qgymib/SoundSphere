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
    "src/main.cpp"
)

if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${soundsphere_sources})
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

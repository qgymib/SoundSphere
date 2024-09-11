set(LIBEV_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libev)

add_library(ev
    ${LIBEV_ROOT}/ev.c
)

target_include_directories(ev
    PUBLIC
        $<BUILD_INTERFACE:${LIBEV_ROOT}>
)

find_package(Threads REQUIRED)
target_link_libraries(ev PRIVATE Threads::Threads)
if(WIN32)
    target_link_libraries(ev PRIVATE Ws2_32 Mswsock)
else()
    target_link_libraries(ev PRIVATE rt dl)
endif()

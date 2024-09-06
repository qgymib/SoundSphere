set(SPDLOG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/spdlog)

add_library(spdlog INTERFACE)
target_include_directories(spdlog
    INTERFACE
        ${SPDLOG_ROOT}/include
)

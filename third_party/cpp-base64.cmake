set(CPP_BASE64_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cpp-base64)

add_library(cpp_base64
    "${CPP_BASE64_ROOT}/base64.cpp"
)
target_include_directories(cpp_base64
    PUBLIC
        $<BUILD_INTERFACE:${CPP_BASE64_ROOT}>
)

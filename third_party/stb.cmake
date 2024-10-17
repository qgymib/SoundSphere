set(STB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb)
set(STB_IMPL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_impl)

add_library(stb
    ${STB_IMPL_ROOT}/stb_impl.c
)
target_include_directories(stb
    PUBLIC
        ${STB_ROOT}
)

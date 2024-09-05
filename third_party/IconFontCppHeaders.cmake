set(ICON_FONT_CPP_HEADERS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/IconFontCppHeaders)

add_library(IconFontCppHeaders INTERFACE)
target_include_directories(IconFontCppHeaders
    INTERFACE
        ${ICON_FONT_CPP_HEADERS_ROOT}
)

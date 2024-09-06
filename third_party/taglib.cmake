set(TAGLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/taglib)
set(TAGLIB_BUILD_DIR ${CMAKE_BINARY_DIR}/TagLib-build)
set(TAGLIB_INSTALL_DIR ${CMAKE_BINARY_DIR}/TagLib-install)

include(ExternalProject)

ExternalProject_Add(3rd_TagLib
    SOURCE_DIR  ${TAGLIB_ROOT}
    BINARY_DIR  ${TAGLIB_BUILD_DIR}
    INSTALL_DIR ${TAGLIB_INSTALL_DIR}
    CMAKE_CACHE_ARGS
        -DBUILD_TESTING:BOOL=OFF
        -DWITH_ZLIB:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

add_library(TagLib IMPORTED STATIC)
target_include_directories(TagLib
    INTERFACE
        ${TAGLIB_INSTALL_DIR}/include
)
set_target_properties(TagLib PROPERTIES
    IMPORTED_LOCATION
        ${TAGLIB_INSTALL_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}tag${CMAKE_STATIC_LIBRARY_SUFFIX}
)

add_dependencies(TagLib 3rd_TagLib)

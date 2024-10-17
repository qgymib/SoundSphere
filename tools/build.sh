#!/bin/bash

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$(dirname "$script_dir")"
work_dir=$project_dir/build.unix
build_dir=$work_dir/build
cache_dir=$work_dir/cache
download_dir=$work_dir/download
install_dir=$work_dir/install

# Prepare build directory.
if [ ! -d "$build_dir" ]; then
    mkdir -p "$build_dir"
fi
if [ ! -d "$cache_dir" ]; then
    mkdir -p "$cache_dir"
fi
if [ ! -d "$download_dir" ]; then
    mkdir -p "$download_dir"
fi
if [ ! -d "$install_dir" ]; then
    mkdir -p "$install_dir"
fi

###############################################################################
# zlib
###############################################################################
zlib_version="1.3.1"
zlib_url="https://www.zlib.net/zlib-$zlib_version.tar.gz"
zlib_file="$download_dir/zlib-$zlib_version.tar.gz"
zlib_uncompress_dir="$cache_dir/zlib-$zlib_version"
zlib_build_dir="$build_dir/zlib-$zlib_version"
zlib_install_dir="$install_dir/zlib-$zlib_version"
if [ ! -f "$zlib_file" ]; then
    wget $zlib_url -O "$zlib_file"
fi
if [ ! -d "$zlib_uncompress_dir" ]; then
    tar zxvf $zlib_file -C $cache_dir
fi
if [ ! -d "$zlib_build_dir" ]; then
    mkdir $zlib_build_dir
fi
(cd $zlib_build_dir && \
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$zlib_install_dir \
        $zlib_uncompress_dir && \
    cmake --build . --config Release && \
    cmake --install .)

###############################################################################
# TagLib
###############################################################################
taglib_version="v2.0.2"
taglib_url="https://github.com/taglib/taglib.git"
taglib_uncompress_dir="$download_dir/taglib"
taglib_build_dir="$build_dir/TagLib-$taglib_version"
taglib_install_dir="$install_dir/TagLib-$taglib_version"
if [ ! -d "$taglib_uncompress_dir" ] || \
    [ "$(cd $taglib_uncompress_dir && git describe --exact-match --tags)" != "$taglib_version" ]; then
    rm -rf $taglib_uncompress_dir
    (cd $download_dir && \
        git clone --recurse-submodules --depth 1 --branch $taglib_version $taglib_url)
fi
if [ ! -d "$taglib_build_dir" ]; then
    mkdir $taglib_build_dir
fi
(cd $taglib_build_dir && \
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$taglib_install_dir \
        -DZLIB_ROOT=$zlib_install_dir \
        -DZLIB_USE_STATIC_LIBS=ON \
        $taglib_uncompress_dir && \
    cmake --build . --config Release && \
    cmake --install .)
if [ $? -ne 0 ]; then
    echo "Build TagLib failed."
    exit 1
fi

###############################################################################
# mbedtls
###############################################################################
mbedtls_version="3.6.1"
mbedtls_url="https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-$mbedtls_version/mbedtls-$mbedtls_version.tar.bz2"
mbedtls_file="$download_dir/mbedtls-$mbedtls_version.tar.bz2"
mbedtls_uncompress_dir="$cache_dir/mbedtls-$mbedtls_version"
mbedtls_build_dir="$build_dir/mbedtls-$mbedtls_version"
mbedtls_install_dir="$install_dir/mbedtls-$mbedtls_version"
if [ ! -f "$mbedtls_file" ]; then
    wget $mbedtls_url -O "$mbedtls_file"
fi
if [ ! -d "$mbedtls_uncompress_dir" ]; then
    tar jxvf $mbedtls_file -C $cache_dir
fi
if [ ! -d "$mbedtls_build_dir" ]; then
    mkdir $mbedtls_build_dir
fi
(cd $mbedtls_build_dir && \
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$mbedtls_install_dir \
        $mbedtls_uncompress_dir && \
    cmake --build . --config Release && \
    cmake --install .)
if [ $? -ne 0 ]; then
    echo "Build mbedTLS failed."
    exit 1
fi

###############################################################################
# curl
###############################################################################
curl_version="8.10.1"
curl_url="https://curl.se/download/curl-$curl_version.tar.gz"
curl_file="$download_dir/curl-$curl_version.tar.gz"
curl_uncompress_dir="$cache_dir/curl-$curl_version"
curl_build_dir="$build_dir/curl-$curl_version"
curl_install_dir="$install_dir/curl-$curl_version"
if [ ! -f "$curl_file" ]; then
    wget $curl_url -O "$curl_file"
fi
if [ ! -d "$curl_uncompress_dir" ]; then
    tar zxvf $curl_file -C $cache_dir
fi
if [ ! -d "$curl_build_dir" ]; then
    mkdir $curl_build_dir
fi
(cd $curl_build_dir && \
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$curl_install_dir \
        -DBUILD_SHARED_LIBS=OFF \
        -DBUILD_STATIC_LIBS=ON \
        -DCURL_USE_LIBSSH2=OFF \
        -DCURL_USE_MBEDTLS=ON \
        -DPC_MBEDTLS_INCLUDE_DIRS=$mbedtls_install_dir/include \
        -DPC_MBEDTLS_LIBRARY_DIRS=$mbedtls_install_dir/lib \
        -DZLIB_ROOT=$zlib_install_dir \
        -DZLIB_USE_STATIC_LIBS=ON \
        $curl_uncompress_dir && \
    cmake --build . --config Release && \
    cmake --install .)

###############################################################################
# SDL2
###############################################################################
sdl2_version="2.30.7"
sdl2_url="https://github.com/libsdl-org/SDL/archive/refs/tags/release-${sdl2_version}.tar.gz"
sdl2_file="$download_dir/SDL-release-${sdl2_version}.tar.gz"
sdl2_uncompress_dir="$cache_dir/SDL-release-${sdl2_version}"
sdl2_build_dir="$build_dir/SDL-${sdl2_version}"
sdl2_install_dir="$install_dir/SDL-${sdl2_version}"
if [ ! -f "$sdl2_file" ]; then
    wget $sdl2_url -O "$sdl2_file"
fi
if [ ! -d "$sdl2_uncompress_dir" ]; then
    tar zxvf $sdl2_file -C $cache_dir
fi
if [ ! -d "$sdl2_build_dir" ]; then
    mkdir $sdl2_build_dir
fi
(cd $sdl2_build_dir && \
    cmake \
        -DCMAKE_INSTALL_PREFIX=$sdl2_install_dir \
        -DCMAKE_BUILD_TYPE=Release \
        $sdl2_uncompress_dir && \
    make && make install)
if [ $? -ne 0 ]; then
    echo "Build SDL2 failed."
    exit 1
fi

###############################################################################
# SDL_mixer
# Require `libasound2-dev`
###############################################################################
sdl_mixer_version="release-2.8.0"
sdl_mixer_url="https://github.com/libsdl-org/SDL_mixer.git"
sdl_mixer_uncompress_dir="$download_dir/SDL_mixer"
sdl_mixer_build_dir="$build_dir/SDL_mixer-$sdl_mixer_version"
sdl_mixer_install_dir="$install_dir/SDL_mixer-$sdl_mixer_version"
if [ ! -d "$sdl_mixer_uncompress_dir" ] || \
    [ "$(cd $sdl_mixer_uncompress_dir && git describe --exact-match --tags)" != "$sdl_mixer_version" ]; then
    (cd $download_dir && git clone --recurse-submodules --depth 1 --branch $sdl_mixer_version $sdl_mixer_url)
fi
if [ ! -d "$sdl_mixer_build_dir" ]; then
    mkdir $sdl_mixer_build_dir
fi
(cd $sdl_mixer_build_dir && \
    cmake \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF \
        -DSDL2MIXER_DEPS_SHARED:BOOL=OFF \
        -DSDL2MIXER_VENDORED:BOOL=ON \
        -DSDL2MIXER_SAMPLES:BOOL=OFF \
        -DCMAKE_INSTALL_PREFIX=$sdl_mixer_install_dir \
        -DCMAKE_BUILD_TYPE=Release \
        -DSDL2_INCLUDE_DIR=$sdl2_install_dir/include/SDL2 \
        -DSDL2_LIBRARY=$sdl2_install_dir/lib/libSDL2.a \
        $sdl_mixer_uncompress_dir && \
    make && make install)
if [ $? -ne 0 ]; then
    echo "Build SDL_mixer failed."
    exit 1
fi

###############################################################################
# FreeType
###############################################################################
freetype_version="2.13.3"
freetype_url="https://download.savannah.gnu.org/releases/freetype/freetype-${freetype_version}.tar.gz"
freetype_file="$download_dir/freetype-${freetype_version}.tar.gz"
freetype_uncompress_dir="$cache_dir/freetype-${freetype_version}"
freetype_build_dir="$build_dir/freetype-${freetype_version}"
freetype_install_dir="$install_dir/freetype-${freetype_version}"
if [ ! -f "$freetype_file" ]; then
    wget $freetype_url -O "$freetype_file"
fi
if [ ! -d "$freetype_uncompress_dir" ]; then
    tar zxvf $freetype_file -C $cache_dir
fi
if [ ! -d "$freetype_build_dir" ]; then
    mkdir $freetype_build_dir
fi
(cd $freetype_build_dir && \
    cmake \
        -DCMAKE_INSTALL_PREFIX=$freetype_install_dir \
        -DCMAKE_BUILD_TYPE=Release \
        -DZLIB_ROOT=$zlib_install_dir \
        -DZLIB_USE_STATIC_LIBS=ON \
        -DFT_DISABLE_PNG=ON \
        -DFT_DISABLE_BZIP2=ON \
        -DFT_DISABLE_BROTLI=ON \
        -DFT_DISABLE_HARFBUZZ=ON \
        $freetype_uncompress_dir && \
    cmake --build . --config Release && \
    cmake --install .)
if [ $? -ne 0 ]; then
    echo "Build FreeType failed."
    exit 1
fi

###############################################################################
# SoundSphere
###############################################################################
soundsphere_build_dir="$build_dir/SoundSphere"
if [ ! -d "$soundsphere_build_dir" ]; then
    mkdir $soundsphere_build_dir
fi
(cd $soundsphere_build_dir && \
    FREETYPE_DIR=$freetype_install_dir \
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -Dtaglib_DIR=$taglib_install_dir/lib/cmake/taglib \
        -DSDL2_DIR=$sdl2_install_dir/lib/cmake/SDL2 \
        -DSDL2_mixer_DIR=$sdl_mixer_install_dir/lib/cmake/SDL2_mixer \
        -DCMAKE_MODULE_PATH=$sdl_mixer_install_dir/lib/cmake/SDL2_mixer \
        -DCURL_DIR=$curl_install_dir/lib/cmake/CURL \
        $project_dir && \
    cmake --build .)
if [ $? -ne 0 ]; then
    echo "Build SoundSphere failed."
    exit 1
fi

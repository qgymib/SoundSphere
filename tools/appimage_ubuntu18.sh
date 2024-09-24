#!/bin/bash

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$(dirname "$script_dir")"
build_dir=$project_dir/build.appimage
app_dir=$build_dir/AppDir
download_dir=$build_dir/download
install_dir=$build_dir/install

linuxdeploy_url="https://github.com/linuxdeploy/linuxdeploy/releases/latest/download/linuxdeploy-x86_64.AppImage"

cmake_version="3.28.6"
cmake_url="https://github.com/Kitware/CMake/releases/download/v${cmake_version}/cmake-${cmake_version}.tar.gz"

sdl2_version="2.30.7"
sdl2_url="https://github.com/libsdl-org/SDL/archive/refs/tags/release-${sdl2_version}.tar.gz"

sdl_mixer_version="release-2.8.0"
sdl_mixer_url="https://github.com/libsdl-org/SDL_mixer.git"

freetype_version="2.13.3"
freetype_url="https://download.savannah.gnu.org/releases/freetype/freetype-${freetype_version}.tar.gz"

# Prepare build directory.
if [ ! -d "$install_dir" ]; then
    mkdir -p "$install_dir"
fi

###############################################################################
# CMake
# Build cmake requires `libssl-dev`: `sudo apt install libssl-dev`
###############################################################################
cmake_binary=$install_dir/cmake/bin/cmake
if [ ! -f "$download_dir/cmake-${cmake_version}.tar.gz" ]; then
    wget $cmake_url -O "$download_dir/cmake-${cmake_version}.tar.gz"
fi
if [ ! -d "$download_dir/cmake-${cmake_version}" ]; then
    (cd $download_dir && tar zxvf cmake-${cmake_version}.tar.gz)
fi
if [ ! -f "$cmake_binary" ]; then
    (cd $download_dir/cmake-${cmake_version} && ./configure --prefix=$install_dir/cmake && make && make install)
fi
if [ $? -ne 0 ]; then
    echo "Build cmake failed."
    exit 1
fi

###############################################################################
# SDL2
# Require:
#   - libegl1-mesa-dev
###############################################################################
if [ ! -f "$download_dir/SDL-release-${sdl2_version}.tar.gz" ]; then
    wget $sdl2_url -O "$download_dir/SDL-release-${sdl2_version}.tar.gz"
fi
if [ ! -d "$download_dir/SDL-release-${sdl2_version}" ]; then
    (cd $download_dir && tar zxvf SDL-release-${sdl2_version}.tar.gz)
fi
if [ ! -d "$download_dir/SDL-release-${sdl2_version}/build" ]; then
    mkdir $download_dir/SDL-release-${sdl2_version}/build
fi
(cd $download_dir/SDL-release-${sdl2_version}/build && \
    $cmake_binary -DCMAKE_INSTALL_PREFIX=$install_dir/SDL2 -DCMAKE_BUILD_TYPE=Release .. && \
    make && make install)
if [ $? -ne 0 ]; then
    echo "Build SDL2 failed."
    exit 1
fi

###############################################################################
# SDL_mixer
# Require `libasound2-dev`
###############################################################################
if [ ! -d "$download_dir/SDL_mixer" ]; then
    (cd $download_dir && git clone --recurse-submodules --depth 1 --branch $sdl_mixer_version $sdl_mixer_url)
fi
if [ ! -d "$download_dir/SDL_mixer/build" ]; then
    mkdir $download_dir/SDL_mixer/build
fi
(cd $download_dir/SDL_mixer/build && \
    $cmake_binary \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF \
        -DSDL2MIXER_DEPS_SHARED:BOOL=OFF \
        -DSDL2MIXER_VENDORED:BOOL=ON \
        -DSDL2MIXER_SAMPLES:BOOL=OFF \
        -DCMAKE_INSTALL_PREFIX=$install_dir/SDL_mixer \
        -DCMAKE_BUILD_TYPE=Release \
        -DSDL2_INCLUDE_DIR=$install_dir/SDL2/include/SDL2 \
        -DSDL2_LIBRARY=$install_dir/SDL2/lib/libSDL2.a \
        .. && \
    make && make install)
if [ $? -ne 0 ]; then
    echo "Build SDL_mixer failed."
    exit 1
fi

###############################################################################
# FreeType
###############################################################################
if [ ! -f "$download_dir/freetype-${freetype_version}.tar.gz" ]; then
    wget $freetype_url -O "$download_dir/freetype-${freetype_version}.tar.gz"
fi
if [ ! -d "$download_dir/freetype-${freetype_version}" ]; then
    (cd $download_dir && tar zxvf freetype-${freetype_version}.tar.gz)
fi
if [ ! -d "$download_dir/freetype-${freetype_version}/build" ]; then
    mkdir $download_dir/freetype-${freetype_version}/build
fi
(cd $download_dir/freetype-${freetype_version}/build && \
    $cmake_binary -DCMAKE_INSTALL_PREFIX=$install_dir/freetype -DCMAKE_BUILD_TYPE=Release .. && \
    make && make install)
if [ $? -ne 0 ]; then
    echo "Build FreeType failed."
    exit 1
fi

###############################################################################
# SoundSphere
###############################################################################
if [ ! -d "$build_dir/SoundSphere" ]; then
    mkdir $build_dir/SoundSphere
fi
(cd $build_dir/SoundSphere && \
    FREETYPE_DIR=$install_dir/freetype \
    $cmake_binary \
        -DCMAKE_BUILD_TYPE=Release \
        -DSDL2_DIR=$install_dir/SDL2/lib/cmake/SDL2 \
        -DSDL2_mixer_DIR=$install_dir/SDL_mixer/lib/cmake/SDL2_mixer \
        -DCMAKE_MODULE_PATH=$install_dir/SDL_mixer/lib/cmake/SDL2_mixer \
        ../.. && \
    $cmake_binary --build .)
if [ $? -ne 0 ]; then
    echo "Build SoundSphere failed."
    exit 1
fi

###############################################################################
# Pack as appimage
###############################################################################
linuxdeploy_binary=$install_dir/linuxdeploy.AppImage
if [ ! -f "$linuxdeploy_binary" ]; then
    wget $linuxdeploy_url -O "$linuxdeploy_binary"
fi
chmod +x $linuxdeploy_binary

# Cleanup
rm -rf $app_dir

# Setup
$linuxdeploy_binary -e $build_dir/SoundSphere/soundsphere -d $project_dir/tools/io.github.qgymib.soundsphere.desktop -i $project_dir/docs/icon_256.png --appdir=$app_dir

mkdir -p $app_dir/usr/share/metainfo
cp $project_dir/tools/io.github.qgymib.soundsphere.metainfo.xml $app_dir/usr/share/metainfo/io.github.qgymib.soundsphere.appdata.xml

(cd $build_dir && $linuxdeploy_binary --appdir=$app_dir --output appimage)
if [ $? -ne 0 ]; then
    echo "Appimage process failed."
    exit 1
fi

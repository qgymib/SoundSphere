#!/bin/bash

# Get the directory where the script is located
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Get the project directory
project_dir="$(dirname "$script_dir")"

# Build directory.
build_dir=$project_dir/build.appimage

# Appimage directory.
appimage_dir=$build_dir/appimage

# Pack tool url.
linuxdeploy_url="https://github.com/linuxdeploy/linuxdeploy/releases/latest/download/linuxdeploy-x86_64.AppImage"

# Prepare build directory.
if [ ! -d "$build_dir" ]; then
    mkdir -p "$build_dir"
fi

# Prepare tool.
if [ ! -f "$build_dir/linuxdeploy.AppImage" ]; then
    wget $linuxdeploy_url -O "$build_dir/linuxdeploy.AppImage"
fi
chmod +x $build_dir/linuxdeploy.AppImage

# Build project.
(cd $build_dir && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .)

# Cleanup
rm -rf $appimage_dir

# Setup
$build_dir/linuxdeploy.AppImage -e $build_dir/soundsphere -d $project_dir/tools/io.github.qgymib.soundsphere.desktop -i $project_dir/docs/icon_256.png --appdir=$appimage_dir

mkdir -p $appimage_dir/usr/share/metainfo
cp $project_dir/tools/io.github.qgymib.soundsphere.metainfo.xml $appimage_dir/usr/share/metainfo/io.github.qgymib.soundsphere.appdata.xml

$build_dir/linuxdeploy.AppImage --appdir=$appimage_dir --output appimage


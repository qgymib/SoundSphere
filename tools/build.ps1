$scriptPath = $MyInvocation.MyCommand.Path
$scriptDir = Split-Path -Parent $scriptPath
$projectDir = Split-Path -Parent $scriptDir
$buildDir = "$projectDir/build.win32"
$downloadDir = "$buildDir/download"
$installDir = "$buildDir/install"

# Create necessary directories
if (-Not (Test-Path "$buildDir")) {
    New-Item -ItemType Directory -Path $buildDir
}
if (-Not (Test-Path "$downloadDir")) {
    New-Item -ItemType Directory -Path $downloadDir
}
if (-Not (Test-Path "$installDir")) {
    New-Item -ItemType Directory -Path $installDir
}

###############################################################################
# 7zr
###############################################################################
$7zrUrl = "https://www.7-zip.org/a/7zr.exe"
$7zrFile = "$installDir/7zr.exe"
if (-Not (Test-Path "$7zrFile")) {
    Invoke-WebRequest -Uri $7zrUrl -OutFile "$7zrFile"
}

###############################################################################
# 7z
###############################################################################
$7zUrl = "https://www.7-zip.org/a/7z2408-extra.7z"
$7zSaveFile = "$downloadDir/7z2408-extra.7z"
$7zInstallDir = "$installDir/7z2408"
if (-Not (Test-Path "$7zSaveFile")) {
    Invoke-WebRequest -Uri $7zUrl -OutFile "$7zSaveFile"
}
if (-Not (Test-Path "$7zInstallDir")) {
    & "$7zrFile" x "$7zSaveFile"  -o"$7zInstallDir"
}
$7z = "$7zInstallDir/7za.exe"

###############################################################################
# mbedtls
###############################################################################
$mbedtlsVersion = "3.6.1"
$mbedtlsUrl = "https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-$mbedtlsVersion/mbedtls-$mbedtlsVersion.tar.bz2"
$mbedtlsSaveFile = "$downloadDir/mbedtls-$mbedtlsVersion.tar.bz2"
$mbedtlsUncompressDir = "$downloadDir/mbedtls-$mbedtlsVersion"
$mbedtlsBuildDir = "$mbedtlsUncompressDir/build"
$mbedtlsInstallDir = "$installDir/mbedtls-$mbedtlsVersion"
if (-Not (Test-Path "$mbedtlsSaveFile")) {
    Invoke-WebRequest -Uri $mbedtlsUrl -OutFile "$mbedtlsSaveFile"
}
if (-Not (Test-Path "$mbedtlsUncompressDir")) {
    & "$7z" x "$mbedtlsSaveFile"  -o"$downloadDir"
}
if (-Not (Test-Path "$mbedtlsBuildDir")) {
    New-Item -ItemType Directory -Path "$mbedtlsBuildDir"
}
Push-Location -Path "$mbedtlsBuildDir"
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$mbedtlsInstallDir" ..
cmake --build . --config Release
cmake --install .
Pop-Location

###############################################################################
# curl
###############################################################################
$curlVersion = "8.10.1"
$curlUrl = "https://curl.se/download/curl-$curlVersion.zip"
$curlFile = "$downloadDir/curl-$curlVersion.zip"
$curlUncompressDir = "$downloadDir/curl-$curlVersion"
$curlBuildDir = "$curlUncompressDir/build"
$curlInstallDir = "$installDir/curl-$curlVersion"
if (-Not (Test-Path "$curlFile")) {
    Invoke-WebRequest -Uri $curlUrl -OutFile "$curlFile"
}
if (-Not (Test-Path "$curlUncompressDir")) {
    & "$7z" x "$curlFile"  -o"$downloadDir"
}
if (-Not (Test-Path "$curlBuildDir")) {
    New-Item -ItemType Directory -Path "$curlBuildDir"
}
Push-Location -Path "$curlBuildDir"
cmake -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$curlInstallDir" `
	-DBUILD_SHARED_LIBS=OFF `
	-DBUILD_STATIC_LIBS=ON `
	-DENABLE_UNICODE=ON `
    -DCURL_USE_LIBSSH2=OFF `
    -DCURL_USE_MBEDTLS=ON `
    -DPC_MBEDTLS_INCLUDE_DIRS="$mbedtlsInstallDir/include" `
    -DPC_MBEDTLS_LIBRARY_DIRS="$mbedtlsInstallDir/lib" `
    ..
cmake --build . --config Release
cmake --install .
Pop-Location

###############################################################################
# zlib
###############################################################################
$zlibVersion = "1.3.1"
$zlibUrl = "https://www.zlib.net/zlib-$zlibVersion.tar.gz"
$zlibFile = "$downloadDir/zlib-$zlibVersion.tar.gz"
$zlibUncompressDir = "$downloadDir/zlib-$zlibVersion"
$zlibBuildDir = "$zlibUncompressDir/build"
$zlibInstallDir = "$installDir/zlib-$zlibVersion"
if (-Not (Test-Path "$zlibFile")) {
    Invoke-WebRequest -Uri $zlibUrl -OutFile "$zlibFile"
}
if (-Not (Test-Path "$zlibUncompressDir")) {
    & "$7z" x "$zlibFile"  -o"$downloadDir"
}
if (-Not (Test-Path "$zlibBuildDir")) {
    New-Item -ItemType Directory -Path "$zlibBuildDir"
}
Push-Location -Path "$zlibBuildDir"
cmake -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$zlibInstallDir" `
    ..
cmake --build . --config Release
cmake --install .
Pop-Location

###############################################################################
# SDL2
###############################################################################
$sdl2Version = "2.30.7"
$sdl2Url = "https://github.com/libsdl-org/SDL/archive/refs/tags/release-$sdl2Version.zip"
$sdl2File = "$downloadDir/SDL2-$sdl2Version.zip"
$sdl2UncompressDir = "$downloadDir/SDL-release-$sdl2Version"
$sdl2BuildDir = "$sdl2UncompressDir/build"
$sdl2InstallDir = "$installDir/SDL-$sdl2Version"
if (-Not (Test-Path "$sdl2File")) {
    Invoke-WebRequest -Uri $sdl2Url -OutFile "$sdl2File"
}
if (-Not (Test-Path "$sdl2UncompressDir")) {
    & "$7z" x "$sdl2File"  -o"$downloadDir"
}
if (-Not (Test-Path "$sdl2BuildDir")) {
    New-Item -ItemType Directory -Path "$sdl2BuildDir"
}
Push-Location -Path "$sdl2BuildDir"
cmake -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$sdl2InstallDir" `
    ..
cmake --build . --config Release
cmake --install .
Pop-Location

###############################################################################
# SDL_mixer
###############################################################################
$SDLMixerVersion = "release-2.8.0"
$SDLMixerUrl = "https://github.com/libsdl-org/SDL_mixer.git"
$SDLMixerDir = "$downloadDir/SDL_mixer"
$SDLMixerBuildDir = "$SDLMixerDir/build"
$SDLMixerInstallDir = "$installDir/SDL_mixer-$SDLMixerVersion"
if (-Not (Test-Path "$SDLMixerDir")) {
    Push-Location -Path "$downloadDir"
    & git clone --recurse-submodules --depth 1 --branch "$SDLMixerVersion" "$SDLMixerUrl"
    Pop-Location
}
if (-Not (Test-Path "$SDLMixerBuildDir")) {
    New-Item -ItemType Directory -Path "$SDLMixerBuildDir"
}
Push-Location -Path "$SDLMixerBuildDir"
cmake -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$SDLMixerInstallDir" `
    -DBUILD_SHARED_LIBS:BOOL=OFF `
    -DBUILD_TESTING:BOOL=OFF `
    -DSDL2MIXER_DEPS_SHARED:BOOL=OFF `
    -DSDL2MIXER_VENDORED:BOOL=ON `
    -DSDL2MIXER_SAMPLES:BOOL=OFF `
    -DSDL2_INCLUDE_DIR="$sdl2InstallDir/include/SDL2" `
    -DSDL2_LIBRARY="$sdl2InstallDir/lib/SDL2-static.lib" `
    ..
cmake --build . --config Release
cmake --install .
Pop-Location
# We need to patch the `SDL2_mixerConfig.cmake`
$SDLMixerPatch = 'list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")'
$SDLMixerContent = Get-Content -Path "$SDLMixerInstallDir/cmake/SDL2_mixerConfig.cmake"
$SDLMixerNewContent = @($SDLMixerPatch) + $SDLMixerContent
$SDLMixerNewContent | Set-Content -Path "$SDLMixerInstallDir/cmake/SDL2_mixerConfig.cmake"

###############################################################################
# FreeType
###############################################################################
$freetypeVersion = "2.13.3"
$freetypeUrl = "https://download.savannah.gnu.org/releases/freetype/freetype-$freetypeVersion.tar.gz"
$freetypeSaveFile = "$downloadDir/freetype-$freetypeVersion.tar.gz"
$freetypeUncompressDir = "$downloadDir/freetype-$freetypeVersion"
$freetypeBuildDir = "$freetypeUncompressDir/build"
$freetypeInstallDir = "$installDir/freetype-$freetypeVersion"
if (-Not (Test-Path "$freetypeSaveFile")) {
    Invoke-WebRequest -Uri $freetypeUrl -OutFile "$freetypeSaveFile"
}
if (-Not (Test-Path "$freetypeUncompressDir")) {
    & "$7z" x "$freetypeSaveFile"  -o"$downloadDir"
    & "$7z" x "$downloadDir/freetype-$freetypeVersion.tar"  -o"$downloadDir"
}
if (-Not (Test-Path "$freetypeBuildDir")) {
    New-Item -ItemType Directory -Path "$freetypeBuildDir"
}
Push-Location -Path "$freetypeBuildDir"
cmake -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$freetypeInstallDir" `
    ..
cmake --build . --config Release
cmake --install .
Pop-Location

###############################################################################
# SoundSphere
###############################################################################
$soundshphereBuildDir = "$buildDir/SoundSphere"
if (-Not (Test-Path "$soundshphereBuildDir")) {
    New-Item -ItemType Directory -Path "$soundshphereBuildDir"
}
Push-Location -Path "$soundshphereBuildDir"
$env:FREETYPE_DIR = "$freetypeInstallDir"
cmake -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$freetypeInstallDir" `
    -DSDL2_DIR="$sdl2InstallDir/cmake" `
    -DSDL2_mixer_DIR="$SDLMixerInstallDir/cmake" `
	-DSDL2MIXER_VENDORED=OFF `
    -DCURL_DIR="$curlInstallDir/lib\cmake\CURL" `
    -DZLIB_ROOT="$zlibInstallDir" `
    -DZLIB_USE_STATIC_LIBS=ON `
    ../..
cmake --build . --config Release
Remove-Item Env:\FREETYPE_DIR
Pop-Location

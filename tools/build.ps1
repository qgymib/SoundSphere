$scriptPath = $MyInvocation.MyCommand.Path
$scriptDir = Split-Path -Parent $scriptPath
$projectDir = Split-Path -Parent $scriptDir
$workDir = "$projectDir/build.win32"
$buildDir = "$workDir/build"
$cacheDir = "$workDir/cache"
$downloadDir = "$workDir/download"
$installDir = "$workDir/install"

# Create necessary directories
if (-Not (Test-Path "$workDir")) {
    New-Item -ItemType Directory -Path $workDir
}
if (-Not (Test-Path "$buildDir")) {
    New-Item -ItemType Directory -Path $buildDir
}
if (-Not (Test-Path "$cacheDir")) {
    New-Item -ItemType Directory -Path $cacheDir
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
$7zrFile = "$downloadDir/7zr.exe"
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
# zlib
###############################################################################
$zlibVersion = "1.3.1"
$zlibUrl = "https://www.zlib.net/zlib-$zlibVersion.tar.gz"
$zlibFile = "$downloadDir/zlib-$zlibVersion.tar.gz"
$zlibUncompressDir = "$cacheDir/zlib-$zlibVersion"
$zlibBuildDir = "$buildDir/zlib-$zlibVersion"
$zlibInstallDir = "$installDir/zlib-$zlibVersion"
if (-Not (Test-Path "$zlibFile")) {
    Invoke-WebRequest -Uri $zlibUrl -OutFile "$zlibFile"
}
if (-Not (Test-Path "$zlibUncompressDir")) {
    & "$7z" x "$zlibFile"  -o"$cacheDir"
	& "$7z" x "$cacheDir/zlib-$zlibVersion.tar"  -o"$cacheDir"
}
if (-Not (Test-Path "$zlibBuildDir")) {
    New-Item -ItemType Directory -Path "$zlibBuildDir"
}
Push-Location -Path "$zlibBuildDir"
cmake -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$zlibInstallDir" `
    "$zlibUncompressDir"
cmake --build . --config Release
cmake --install .
Pop-Location

###############################################################################
# TagLib
###############################################################################
$taglibVersion = "v2.0.2"
$taglibUrl = "https://github.com/taglib/taglib.git"
$taglibUncompressDir = "$downloadDir/taglib"
$taglibBuildDir = "$buildDir/TagLib-$taglibVersion"
$taglibInstallDir = "$installDir/TagLib-$taglibVersion"
if (-Not (Test-Path "$taglibUncompressDir")) {
    Push-Location -Path "$downloadDir"
    & git clone --recurse-submodules --depth 1 --branch "$taglibVersion" "$taglibUrl"
    Pop-Location
}
if (-Not (Test-Path "$taglibBuildDir")) {
    New-Item -ItemType Directory -Path "$taglibBuildDir"
}
Push-Location -Path "$taglibBuildDir"
cmake `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$taglibInstallDir" `
    -DZLIB_ROOT="$zlibInstallDir" `
    -DZLIB_USE_STATIC_LIBS=ON `
    "$taglibUncompressDir"
cmake --build . --config Release
cmake --install .
Pop-Location

###############################################################################
# mbedtls
###############################################################################
$mbedtlsVersion = "3.6.1"
$mbedtlsUrl = "https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-$mbedtlsVersion/mbedtls-$mbedtlsVersion.tar.bz2"
$mbedtlsSaveFile = "$downloadDir/mbedtls-$mbedtlsVersion.tar.bz2"
$mbedtlsUncompressDir = "$cacheDir/mbedtls-$mbedtlsVersion"
$mbedtlsBuildDir = "$buildDir/mbedtls-$mbedtlsVersion"
$mbedtlsInstallDir = "$installDir/mbedtls-$mbedtlsVersion"
if (-Not (Test-Path "$mbedtlsSaveFile")) {
    Invoke-WebRequest -Uri $mbedtlsUrl -OutFile "$mbedtlsSaveFile"
}
if (-Not (Test-Path "$mbedtlsUncompressDir")) {
    & "$7z" x "$mbedtlsSaveFile"  -o"$cacheDir"
	& "$7z" x "$cacheDir/mbedtls-$mbedtlsVersion.tar"  -o"$cacheDir"
}
if (-Not (Test-Path "$mbedtlsBuildDir")) {
    New-Item -ItemType Directory -Path "$mbedtlsBuildDir"
}
Push-Location -Path "$mbedtlsBuildDir"
cmake `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$mbedtlsInstallDir" `
    "$mbedtlsUncompressDir"
cmake --build . --config Release
cmake --install .
Pop-Location

###############################################################################
# curl
###############################################################################
$curlVersion = "8.10.1"
$curlUrl = "https://curl.se/download/curl-$curlVersion.zip"
$curlFile = "$downloadDir/curl-$curlVersion.zip"
$curlUncompressDir = "$cacheDir/curl-$curlVersion"
$curlBuildDir = "$buildDir/curl-$curlVersion"
$curlInstallDir = "$installDir/curl-$curlVersion"
if (-Not (Test-Path "$curlFile")) {
    Invoke-WebRequest -Uri $curlUrl -OutFile "$curlFile"
}
if (-Not (Test-Path "$curlUncompressDir")) {
    & "$7z" x "$curlFile"  -o"$cacheDir"
}
if (-Not (Test-Path "$curlBuildDir")) {
    New-Item -ItemType Directory -Path "$curlBuildDir"
}
Push-Location -Path "$curlBuildDir"
cmake `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$curlInstallDir" `
	-DBUILD_SHARED_LIBS=OFF `
	-DBUILD_STATIC_LIBS=ON `
	-DENABLE_UNICODE=ON `
    -DCURL_USE_LIBSSH2=OFF `
    -DCURL_USE_MBEDTLS=ON `
    -DPC_MBEDTLS_INCLUDE_DIRS="$mbedtlsInstallDir/include" `
    -DPC_MBEDTLS_LIBRARY_DIRS="$mbedtlsInstallDir/lib" `
    -DZLIB_ROOT="$zlibInstallDir" `
    -DZLIB_USE_STATIC_LIBS=ON `
    "$curlUncompressDir"
cmake --build . --config Release
cmake --install .
Pop-Location

###############################################################################
# SDL2
###############################################################################
$sdl2Version = "2.30.7"
$sdl2Url = "https://github.com/libsdl-org/SDL/archive/refs/tags/release-$sdl2Version.zip"
$sdl2File = "$downloadDir/SDL2-$sdl2Version.zip"
$sdl2UncompressDir = "$cacheDir/SDL-release-$sdl2Version"
$sdl2BuildDir = "$buildDir/SDL-$sdl2Version"
$sdl2InstallDir = "$installDir/SDL-$sdl2Version"
if (-Not (Test-Path "$sdl2File")) {
    Invoke-WebRequest -Uri $sdl2Url -OutFile "$sdl2File"
}
if (-Not (Test-Path "$sdl2UncompressDir")) {
    & "$7z" x "$sdl2File"  -o"$cacheDir"
}
if (-Not (Test-Path "$sdl2BuildDir")) {
    New-Item -ItemType Directory -Path "$sdl2BuildDir"
}
Push-Location -Path "$sdl2BuildDir"
cmake `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$sdl2InstallDir" `
    "$sdl2UncompressDir"
cmake --build . --config Release
cmake --install .
Pop-Location

###############################################################################
# SDL_mixer
###############################################################################
$SDLMixerVersion = "release-2.8.0"
$SDLMixerUrl = "https://github.com/libsdl-org/SDL_mixer.git"
$SDLMixerUncompressDir = "$downloadDir/SDL_mixer"
$SDLMixerBuildDir = "$buildDir/SDL_mixer-$SDLMixerVersion"
$SDLMixerInstallDir = "$installDir/SDL_mixer-$SDLMixerVersion"
if (-Not (Test-Path "$SDLMixerUncompressDir")) {
    Push-Location -Path "$downloadDir"
    & git clone --recurse-submodules --depth 1 --branch "$SDLMixerVersion" "$SDLMixerUrl"
    Pop-Location
}
if (-Not (Test-Path "$SDLMixerBuildDir")) {
    New-Item -ItemType Directory -Path "$SDLMixerBuildDir"
}
Push-Location -Path "$SDLMixerBuildDir"
cmake `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$SDLMixerInstallDir" `
    -DBUILD_SHARED_LIBS:BOOL=OFF `
    -DBUILD_TESTING:BOOL=OFF `
    -DSDL2MIXER_DEPS_SHARED:BOOL=OFF `
    -DSDL2MIXER_VENDORED:BOOL=ON `
    -DSDL2MIXER_SAMPLES:BOOL=OFF `
    -DSDL2_INCLUDE_DIR="$sdl2InstallDir/include/SDL2" `
    -DSDL2_LIBRARY="$sdl2InstallDir/lib/SDL2-static.lib" `
    "$SDLMixerUncompressDir"
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
$freetypeUncompressDir = "$cacheDir/freetype-$freetypeVersion"
$freetypeBuildDir = "$buildDir/freetype-$freetypeVersion"
$freetypeInstallDir = "$installDir/freetype-$freetypeVersion"
if (-Not (Test-Path "$freetypeSaveFile")) {
    Invoke-WebRequest -Uri $freetypeUrl -OutFile "$freetypeSaveFile"
}
if (-Not (Test-Path "$freetypeUncompressDir")) {
    & "$7z" x "$freetypeSaveFile"  -o"$cacheDir"
    & "$7z" x "$cacheDir/freetype-$freetypeVersion.tar"  -o"$cacheDir"
}
if (-Not (Test-Path "$freetypeBuildDir")) {
    New-Item -ItemType Directory -Path "$freetypeBuildDir"
}
Push-Location -Path "$freetypeBuildDir"
cmake `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$freetypeInstallDir" `
    "$freetypeUncompressDir"
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
cmake `
    -DCMAKE_BUILD_TYPE=Release `
	-Dtaglib_DIR="$taglibInstallDir/lib/cmake/taglib" `
    -DSDL2_DIR="$sdl2InstallDir/cmake" `
    -DSDL2_mixer_DIR="$SDLMixerInstallDir/cmake" `
    -DCURL_DIR="$curlInstallDir/lib/cmake/CURL" `
    "$projectDir"
cmake --build . --config Release
Remove-Item Env:\FREETYPE_DIR
Pop-Location

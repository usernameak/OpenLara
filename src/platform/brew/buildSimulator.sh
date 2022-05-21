#!/bin/bash

# Should be built with mingw-w64-x86_64-gcc

set -e

if [ -z "$SDK_PATH" ]; then
    >&2 echo 'Cannot build: SDK_PATH is not set'
    >&2 echo 'export platform SDK subfolder `sdk` as SDK_PATH'
    exit 1
fi

if [ -z "$BREW_GL_SDK_PATH" ]; then
    >&2 echo 'Cannot build: BREW_GL_SDK_PATH is not set'
    >&2 echo 'export OpenGL ES extension SDK as BREW_GL_SDK_PATH'
    exit 1
fi

# SDK_PATH=/d/src/BREW/BREW_3.1.5_SP02_PLATFORM/sdk/

BINDIR=../../../bin/brewsim/
OBJDIR=$BINDIR/obj

mkdir -p $BINDIR
mkdir -p $OBJDIR
mkdir -p $BINDIR/mod/OpenLara
mkdir -p $BINDIR/mif/

CFLAGS="-O0 -fshort-wchar -m32 -DAEE_SIMULATOR \
        -ffunction-sections -fdata-sections \
        -fno-exceptions -I$SDK_PATH/inc -I../../ -I$BREW_GL_SDK_PATH/inc \
        -fdiagnostics-color \
        -D__ARMCGCC -DDYNAMIC_APP -D__BREW__ -DSTB_VORBIS_NO_STDIO"
LDFLAGS="${CFLAGS} -shared"
CXXFLAGS="$CFLAGS -fno-use-cxa-atexit -fno-rtti"
LDLIBS="-lm"

function compileFileC {
    local sourceName=${1}

    gcc $CFLAGS -o $OBJDIR/$(basename ${sourceName%.*}.o) -c $sourceName
}

function compileFileCPP {
    local sourceName=${1}

    g++ $CXXFLAGS -o $OBJDIR/$(basename ${sourceName%.*}.o) -c $sourceName
}

compileFileCPP main.cpp
compileFileC brewFileIO.c
compileFileC $SDK_PATH/src/AEEAppGen.c
#compileFileC $BREW_GL_SDK_PATH/src/GLES_1x.c
compileFileC $BREW_GL_SDK_PATH/src/GLES_1x.c
compileFileC $BREW_GL_SDK_PATH/src/GLES_ext.c
compileFileC $BREW_GL_SDK_PATH/src/EGL_1x.c
compileFileC OpenLaraMod.c
compileFileC ../../libs/stb_vorbis/stb_vorbis.c
compileFileCPP ../../libs/minimp3/minimp3.cpp
compileFileC ../../libs/tinf/tinflate.c

gcc $LDFLAGS -o$BINDIR/mod/OpenLara/OpenLara.dll \
    $OBJDIR/brewFileIO.o \
    $OBJDIR/main.o \
    $OBJDIR/AEEAppGen.o \
    $OBJDIR/OpenLaraMod.o \
    $OBJDIR/stb_vorbis.o \
    $OBJDIR/tinflate.o \
    $OBJDIR/minimp3.o \
    $OBJDIR/GLES_1x.o \
    $OBJDIR/GLES_ext.o \
    $OBJDIR/EGL_1x.o \
    $LDLIBS
#./elf2mod.exe -output $BINDIR/mod/OpenLara/OpenLara.mod $OBJDIR/OpenLara.elf

cp OpenLara.mif $BINDIR/mif/OpenLara.mif

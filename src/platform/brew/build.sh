#!/bin/bash

# Should be built with mingw-w64-x86_64-arm-none-eabi-gcc

set -e

if [ -z "$SDK_PATH" ]; then
    >&2 echo 'Cannot build: SDK_PATH is not set'
    >&2 echo 'export platform SDK subfolder `sdk` as SDK_PATH'
    exit 1
fi

# SDK_PATH=/d/src/BREW/BREW_3.1.5_SP02_PLATFORM/sdk/
BINDIR=../../../bin/brew/
OBJDIR=$BINDIR/obj

mkdir -p $BINDIR
mkdir -p $OBJDIR
mkdir -p $BINDIR/mod/OpenLara
mkdir -p $BINDIR/mif/

CFLAGS="-Os -fshort-wchar -mword-relocations \
        -ffunction-sections -fdata-sections \
        -fno-exceptions -marm -mthumb-interwork \
        -march=armv5te -I$SDK_PATH/inc -I../../ \
        -fdiagnostics-color -specs=nosys.specs \
        -D__ARMCGCC -DDYNAMIC_APP -D__BREW__ -DSTB_VORBIS_NO_STDIO"
LDFLAGS="${CFLAGS} -nostartfiles -Wl,--entry=AEEMod_Load -Wl,--emit-relocs  \
              -Wl,--default-script=elf2mod.x \
			  -Wl,--no-wchar-size-warning -static-libgcc -Wl,--gc-sections"
CXXFLAGS="$CFLAGS -fno-use-cxa-atexit -fno-rtti"
LDLIBS="-lm"

function compileFileC {
    local sourceName=${1}

    arm-none-eabi-gcc $CFLAGS -o $OBJDIR/$(basename ${sourceName%.*}.o) -c $sourceName
}

function compileFileCPP {
    local sourceName=${1}

    arm-none-eabi-g++ $CXXFLAGS -o $OBJDIR/$(basename ${sourceName%.*}.o) -c $sourceName
}

compileFileCPP main.cpp
compileFileC brewFileIO.c
compileFileC $SDK_PATH/src/AEEAppGen.c
compileFileC OpenLaraMod.c
compileFileC ../../libs/stb_vorbis/stb_vorbis.c
compileFileCPP ../../libs/minimp3/minimp3.cpp
compileFileC ../../libs/tinf/tinflate.c

arm-none-eabi-gcc $LDFLAGS -o$OBJDIR/OpenLara.elf \
    $OBJDIR/brewFileIO.o \
    $OBJDIR/main.o \
    $OBJDIR/AEEAppGen.o \
    $OBJDIR/OpenLaraMod.o \
    $OBJDIR/stb_vorbis.o \
    $OBJDIR/tinflate.o \
    $OBJDIR/minimp3.o \
    $LDLIBS
./elf2mod.exe -output $BINDIR/mod/OpenLara/OpenLara.mod $OBJDIR/OpenLara.elf

cp OpenLara.mif $BINDIR/mif/OpenLara.mif

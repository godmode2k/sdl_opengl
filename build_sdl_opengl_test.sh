#!/bin/sh


BIN_CC=/usr/bin/g++


INC="-I./"
LIBS_INC=""
OPT_SDL_V1="-D__SDLv1__"
OPT_SDL_V2="-D__SDLv2__"
OPT_SDL_VERSION="$OPT_SDL_V1"
LIBS="
	-lSDL		\
	-lSDL_mixer	\
"
SRC_FILES="
"
DEBUG="-g"
#OPT_VERBOSE="-Wl,-verbose"
CFLAGS="$DEBUG"
# C++11
OPT_CPP11="-std=gnu++0x"
#OPT_CPP11="-std=c++0x"
#OPT_CPP11="-std=c++11"
OPT_CPP="$OPT_CPP11"
OPTS="-Wall -O2 $CFLAGS $OPT_CPP $OPT_SDL_VERSION $INC $LIBS"


if [ -z "$1" ] || [ -z "$2" ]; then
	echo $0
	echo "Usage: sh $0 {output} {input}"
	echo ""
	exit
fi

OUTPUT=$1
INPUT=$2
BUILD="$BIN_CC -o $OUTPUT $INPUT $SRC_FILES $OPTS"

echo '---------------'
echo 'Build...'
echo '---------------'
echo $BUILD
$BUILD

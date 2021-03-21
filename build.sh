#!/bin/bash
# ^ bash is required for the "if" syntax used here.
if [ -z "$PREFIX" ]; then
    PREFIX="/usr"
fi
mkdir -p $PREFIX || exit 1

if [ -z "$DEBUG" ]; then
    DEBUG=false
fi
if [ "@$1" == "@--debug" ]; then
    DEBUG=true
fi
OPTION1="-O2"
OPTION2=
OPTION3=
if [ "@$DEBUG" = "@true" ]; then
    OPTION1="-g"
    #OPTION2="-DQT_QML_DEBUG"
    OPTION3="-DDEBUG=true"
    echo "* build:Debug"
else
    echo "* build:Release"
fi
#IRR_INCDIR=
#IRR_LIBDIR=
# FT2_INCDIR=$PREFIX/include/freetype2
FT2_INCDIR=$PREFIX/include/freetype2
OUT_BIN=build/b3view
#FT2_LIBDIR=
OBJDIR="./build/tmp"

# ^ build is in .gitignore

if [ ! -d "$OBJDIR" ]; then
    mkdir -p "$OBJDIR"
fi

# only for pc file if exists for irrlicht:
#if [ -z "$PKG_CONFIG_PATH" ]; then
#    PKG_CONFIG_PATH=$IRR_PATH
#else
#    PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$IRR_PATH
#fi
#gcc -o build/b3view main.cpp Debug.cpp Engine.cpp EventHandler.cpp settings.cpp  UserInterface.cpp  Utility.cpp  View.cpp $(pkg-config --libs --cflags irrlicht --cflags freetype2)
#^ can't find a pc file
# gcc -o build/b3view main.cpp Debug.cpp Engine.cpp EventHandler.cpp settings.cpp  UserInterface.cpp  Utility.cpp  View.cpp -I$FT2_INCDIR

# based on qtcreator's build after clean (see contributing.md; some options are unclear):
g++ -c -pipe $OPTION1 $OPTION2 $OPTION3 -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/main.o ../b3view/main.cpp
if [ $? -ne 0 ]; then
    echo "Error: building main failed. Ensure that libirrlicht-dev is installed."
    exit 1
fi
g++ -c -pipe $OPTION1 $OPTION2 $OPTION3 -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/Engine.o ../b3view/Engine.cpp
g++ -c -pipe $OPTION1 $OPTION2 $OPTION3 -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/EventHandler.o ../b3view/EventHandler.cpp
g++ -c -pipe $OPTION1 $OPTION2 $OPTION3 -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/UserInterface.o ../b3view/UserInterface.cpp
g++ -c -pipe $OPTION1 $OPTION2 $OPTION3 -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/View.o ../b3view/View.cpp
g++ -c -pipe $OPTION1 $OPTION2 $OPTION3 -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/Debug.o ../b3view/Debug.cpp
g++ -c -pipe $OPTION1 $OPTION2 $OPTION3 -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/CGUITTFont.o ../b3view/extlib/CGUITTFont.cpp
g++ -c -pipe $OPTION1 $OPTION2 $OPTION3 -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/Utility.o ../b3view/Utility.cpp
g++ -c -pipe $OPTION1 $OPTION2 $OPTION3 -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/settings.o ../b3view/settings.cpp
#-w: suppress warning
# -I.: include the current directory (suppresses errors when using include < instead of include "
#-pipe: "Use pipes rather than intermediate files."
#Options starting with -g, -f, -m, -O, -W, or --param are automatically
# passed on to the various sub-processes invoked by g++.  In order to pass
# other options on to these processes the -W<letter> options must be used.
if [ -f "$$OUT_BIN" ]; then
    mv "$OUT_BIN" "$OUT_BIN.BAK"
    if [ $? -ne 0 ]; then
        echo "Error: 'mv \"$OUT_BIN\" \"$OUT_BIN.BAK\"' failed.."
        exit 1
    fi
fi
g++  -o build/b3view $OBJDIR/main.o $OBJDIR/Engine.o $OBJDIR/EventHandler.o $OBJDIR/UserInterface.o $OBJDIR/Debug.o $OBJDIR/View.o $OBJDIR/CGUITTFont.o $OBJDIR/Utility.o $OBJDIR/settings.o -lIrrlicht -lX11 -lGL -lXxf86vm -lXcursor -lstdc++fs -lfreetype
if [ $? -ne 0 ]; then
    echo "* linking failed."
else
    echo "* linking suceeded."
fi
if [ ! -f "$OUT_BIN" ]; then
    echo "Error: $OUT_BIN couldn't be built."
    exit 1
else
    echo "Building $OUT_BIN is complete."
fi

INSTALLED_BIN="$HOME/.local/bin/b3view"
if [ -f "$INSTALLED_BIN" ]; then
    echo "* updating $INSTALLED_BIN..."
    ./$OUT_BIN
    if [ $? -eq 0 ]; then
        # if no errors occur, install it
        rm "$INSTALLED_BIN"
        cp -f "$OUT_BIN" "$INSTALLED_BIN"
        if [ $? -eq 0 ]; then
            echo "* installed $INSTALLED_BIN successfully."
        else
            echo "* FAILED to install $INSTALLED_BIN."
        fi
    else
        echo "* skipping install since './$OUT_BIN' failed."
        echo "* try:"
        echo "  $0 --debug"
        echo "  # then:"
        echo "  gdb \"$OUT_BIN\""
    fi
fi

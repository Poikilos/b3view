#!/bin/sh
if [ -z "$PREFIX" ]; then
    PREFIX="/usr"
fi
#IRR_INCDIR=
#IRR_LIBDIR=
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

# Qmake does the following:
#g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/main.o ../b3view/main.cpp
#g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/Engine.o ../b3view/Engine.cpp
#g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/EventHandler.o ../b3view/EventHandler.cpp
#g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/UserInterface.o ../b3view/UserInterface.cpp
#g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/Debug.o ../b3view/Debug.cpp
#g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/View.o ../b3view/View.cpp
#g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/CGUITTFont.o ../b3view/extlib/CGUITTFont.cpp
#g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/Utility.o ../b3view/Utility.cpp
#g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/settings.o ../b3view/settings.cpp
#g++  -o build/b3view tmp/main.o tmp/Engine.o tmp/EventHandler.o tmp/UserInterface.o tmp/Debug.o tmp/View.o tmp/CGUITTFont.o tmp/Utility.o tmp/settings.o   -lIrrlicht -lX11 -lGL -lXxf86vm -lXcursor -lstdc++fs -lfreetype

# based on the above (some options are unclear):
g++ -c -pipe -g -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/main.o ../b3view/main.cpp
g++ -c -pipe -g -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/Engine.o ../b3view/Engine.cpp
g++ -c -pipe -g -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/EventHandler.o ../b3view/EventHandler.cpp
g++ -c -pipe -g -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/UserInterface.o ../b3view/UserInterface.cpp
g++ -c -pipe -g -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/View.o ../b3view/View.cpp
g++ -c -pipe -g -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/Debug.o ../b3view/Debug.cpp
g++ -c -pipe -g -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/CGUITTFont.o ../b3view/extlib/CGUITTFont.cpp
g++ -c -pipe -g -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/Utility.o ../b3view/Utility.cpp
g++ -c -pipe -g -fPIC -I../b3view -I$FT2_INCDIR -o $OBJDIR/settings.o ../b3view/settings.cpp
#-w: suppress warning
# -I.: include the current directory (suppresses errors when using include < instead of include "
#-pipe: "Use pipes rather than intermediate files."
#Options starting with -g, -f, -m, -O, -W, or --param are automatically
# passed on to the various sub-processes invoked by g++.  In order to pass
# other options on to these processes the -W<letter> options must be used.
rm "$OUT_BIN"
if [ -f "$OUT_BIN" ]; then
    echo "Error: $OUT_BIN couldn't be deleted."
    exit 1
fi
g++  -o build/b3view $OBJDIR/main.o $OBJDIR/Engine.o $OBJDIR/EventHandler.o $OBJDIR/UserInterface.o $OBJDIR/Debug.o $OBJDIR/View.o $OBJDIR/CGUITTFont.o $OBJDIR/Utility.o $OBJDIR/settings.o -lIrrlicht -lX11 -lGL -lXxf86vm -lXcursor -lstdc++fs -lfreetype
if [ ! -f "$OUT_BIN" ]; then
    echo "Error: $OUT_BIN couldn't be built."
    exit 1
else
    echo "Building $OUT_BIN is complete."
fi

INSTALLED_BIN="$HOME/.local/bin/b3view"
if [ -f "$INSTALLED_BIN" ]; then
    echo "* updating $INSTALLED_BIN..."
    rm "$INSTALLED_BIN"
    cp -f "$OUT_BIN" "$INSTALLED_BIN"
fi

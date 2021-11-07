#!/bin/bash
# This file should be identical in all projects. See documentation
# under "usage" below.

usage(){
    cat <<END
build.sh
(c) 2021 Poikilos
License of this file: [MIT License](https://opensource.org/licenses/MIT)

This is a modular script to build C++ programs without any make or build tools.

This script is designed to run using a build-settings.rc file containing settings,
or by having all of the settings set in the environment.

To run this script without bash, your shell must be able to process bash-style "if" syntax as used here.


Settings:
    SRC_PATH is the full path to the repository's source code.

    IN_FILES should be a space-separated list of cpp files not including the extension.
    The full path to each file must not include spaces, but the path must be
    relative to the SRC_PATH (since the filenames are collected in a bash
    string then used as the input param for g++).
    If the file is in a subdirectory, you must also specify the name of the
    directory in OTHER_DIRS so a corresponding directory under the objects
    directory gets created (or you simply do that and not specify the subdir
    here, and that option will cause the file to be found--see OTHER_DIRS
    below).

    BIN_NAME is the filename for the binary to compile.

    OTHER_DIRS (optional) is a list of other subdirectories where files may exist.
    The filenames (without extension) must not be the same as in any other
    folder (since this script places all o files in the same directory).
    Each directory must be free of spaces since it is iterated based on spaces.
    Any matching file in such a directory will override the file in the SRC_PATH
    directory if the file name is the same, so there will be no conflict in that
    case, but the file in the SRC_PATH will be ignored.


    DIST_FILES (optional) is a space-separated list of files or directories that should
    be copied to the build directory along with the binary. This is for
    required files that should be distributed with the program.

    BUILD_DIR (optional, default is "build") is the directory to place the binary
    executable and DIST_FILES if any.

Example build-settings.rc:
    SRC_PATH="$REPO_PATH/src"
    IN_FILES="Application ApplicationDelegate IrrlichtEventReceiver main SaveFileDialog"
    BIN_NAME=my_program
    #OTHER_DIRS="extlib"
    #DIST_FILES="media"

With the correct use of a build-settings.rc file,
This script is identical in every project.
For examples, see the following repos by Poikilos on GitHub:
- b3view
- irrPaint3D

END
}

if [ -z "$REPO_PATH" ]; then
    REPO_PATH="`pwd`"
fi

HAS_ALL=false
if [ ! -z "$SRC_PATH" ]; then
    if [ ! -z "$IN_FILES" ]; then
        if [ ! -z "$BIN_NAME" ]; then
            HAS_ALL=true
        fi
    fi
fi

if [ "@$HAS_ALL" != "@true" ]; then
    . build-settings.rc
    if [ ! -f build-settings.rc ]; then
        usage
        echo "Error: build-settings.rc is missing."
        exit 1
    fi
else
    echo "INFO: The variables are in the environment, so checking for build-settings.rc was skipped."
fi

if [ -z "$SRC_PATH" ]; then
    cat <<END

    Error: You must set SRC_PATH in build-settings.rc or the environment.

END
    exit 1
fi
if [ -z "$IN_FILES" ]; then
    cat <<END

Error: You must set IN_FILES in build-settings.rc or the environment.

END
    exit 1
fi
if [ -z "$BIN_NAME" ]; then
    cat <<END

Error: You must set BIN_NAME in build-settings.rc or the environment.
END
    exit 1
fi


if [ -z "$PREFIX" ]; then
    PREFIX="/usr"
fi
mkdir -p $PREFIX
if [ $? -ne 0 ]; then
    echo "Error: 'mkdir -p $PREFIX' failed."
    exit 1
fi

if [ -z "$DEBUG" ]; then
    DEBUG=false
fi
RUN_DEBUG=false
for arg in "$@"
do
    if [ "@$arg" == "@--debug" ]; then
        DEBUG=true
    elif [ "@$arg" == "@--run-debug" ]; then
        DEBUG=true
        RUN_DEBUG=true
    fi
done
OPTION1="-O2"
OPTION2=
OPTION3=

if [ -z "$BUILD_DIR" ]; then
    BUILD_DIR="build"
fi

OUT_BIN="$BUILD_DIR/$BIN_NAME"
OUT_BIN_PATH="`realpath $OUT_BIN`"
OUT_BIN_CWD="`realpath $BUILD_DIR`"

if [ "@$DEBUG" = "@true" ]; then
    OPTION1="-g"
    #OPTION2="-DQT_QML_DEBUG"
    OPTION3="-DDEBUG=true"
    OUT_BIN=build/debug/$BIN_NAME
    mkdir -p "build/debug"
    echo "* build:Debug (`pwd`/$OUT_BIN)"
else
    echo "* build:Release (`pwd`/$OUT_BIN)"
fi
SYSTEM_INCDIR=$PREFIX/include
#IRR_INCDIR=
#IRR_LIBDIR=
# FT2_INCDIR=$PREFIX/include/freetype2
FT2_INCDIR=$PREFIX/include/freetype2
#FT2_LIBDIR=
OBJDIR="./build/tmp"

# ^ build is in .gitignore

if [ ! -d "$OBJDIR" ]; then
    mkdir -p "$OBJDIR"
fi

for extra_dir in $OTHER_DIRS
do
    mkdir -p $OBJDIR/$extra_dir
done

_O_FILES=""
_I_FILES=""
for fn in $IN_FILES
do
    echo "* [$0] compiling $fn..."
    # based on qtcreator's build after clean (see contributing.md; some options are unclear):
    THIS_NAME=$fn.cpp
    THIS_PATH=$SRC_PATH/$THIS_NAME
    THIS_OBJ_PATH="$OBJDIR/$fn.o"
    for extra_dir in $OTHER_DIRS
    do
        if [ -f "$SRC_PATH/$extra_dir/$fn.cpp" ]; then
            THIS_NAME=$extra_dir/$fn.cpp
            THIS_PATH=$SRC_PATH/$THIS_NAME
            break
        fi
    done
    _I_FILES="$_I_FILES $THIS_PATH"
    g++ -c -pipe $OPTION1 $OPTION2 $OPTION3 -fPIC -I$REPO_PATH -I$FT2_INCDIR -o $THIS_OBJ_PATH $THIS_PATH
    if [ $? -ne 0 ]; then echo "Error: building $fn failed."; exit 1; fi
    #-w: suppress warning
    # -I.: include the current directory (suppresses errors when using include < instead of include "
    #-pipe: "Use pipes rather than intermediate files."
    #Options starting with -g, -f, -m, -O, -W, or --param are automatically
    # passed on to the various sub-processes invoked by g++.  In order to pass
    # other options on to these processes the -W<letter> options must be used.
    _O_FILES="$_O_FILES $THIS_OBJ_PATH"
done

# only for pc file if exists for irrlicht:
#if [ -z "$PKG_CONFIG_PATH" ]; then
#    PKG_CONFIG_PATH=$IRR_PATH
#else
#    PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$IRR_PATH
#fi
#gcc -o build/$BIN_NAME $_I_FILES $(pkg-config --libs --cflags irrlicht --cflags freetype2)
#^ can't find a pc file
# gcc -o build/$BIN_NAME $_I_FILES -I$FT2_INCDIR


if [ -f "$$OUT_BIN" ]; then
    mv "$OUT_BIN" "$OUT_BIN.BAK"
    if [ $? -ne 0 ]; then
        echo "Error: 'mv \"$OUT_BIN\" \"$OUT_BIN.BAK\"' failed.."
        exit 1
    fi
fi
g++  -o $OUT_BIN $_O_FILES -lIrrlicht -lX11 -lGL -lXxf86vm -lXcursor -lstdc++fs -lfreetype
if [ $? -ne 0 ]; then
    cat <<END
Error: Linking failed. Ensure you have installed:
- irrlicht-devel and its dependencies: mesa-libGL-devel (requires libglvnd-devel which requires libX11-devel) libXxf86vm-devel
- libXcursor-devel
- freetype-devel
END
    exit 1
else
    echo "* linking succeeded."
fi
if [ ! -f "$OUT_BIN" ]; then
    echo "Error: $OUT_BIN couldn't be built."
    exit 1
else
    echo "Building $OUT_BIN is complete."
fi

if [ ! -z "$DIST_FILES" ]; then
    echo "* copying dist files..."
    for fn in $DIST_FILES
    do
        printf "  * $fn..."
        if [ -d "$fn" ]; then
            cp -R "$fn" $BUILD_DIR/
            if [ $? -ne 0 ]; then
                echo "Error: 'cp -R \"$fn\" $BUILD_DIR/' failed in \"`pwd`\"."
                exit 1
            else
                echo "OK"
            fi
        else
            cp -f "$fn" $BUILD_DIR/
            if [ $? -ne 0 ]; then
                echo "Error: 'cp -f \"$fn\" $BUILD_DIR/' failed in \"`pwd`\"."
                exit 1
            else
                echo "OK"
            fi
        fi
    done
fi

INSTALLED_BIN="$HOME/.local/bin/$BIN_NAME"

if [ "@$RUN_DEBUG" = "@true" ]; then
    if [ "@$DEBUG" = "@true" ]; then
        ./debug-and-show-tb.sh
    else
        echo "Error: you specified the run option but debug mode is not enabled so ./debug-and-show-tb.sh will not work as expected. Try:"
        echo "  $0 --debug --run-debug"
        exit 1
    fi
else
    echo "  (add the --run-debug option to run it automatically)"
fi
if [ "@$DEBUG" != "@true" ]; then
    if [ -f "$INSTALLED_BIN" ]; then
        echo "* updating $INSTALLED_BIN..."
        ./$OUT_BIN --test-and-exit
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
            echo
            if [ "@$DEBUG" != "@true" ]; then
                echo "* Build with debugging symbols like:"
                echo "  $0 --debug"
                echo "  # then:"
            fi
            echo "  # (See ./debug-and-show-tb.sh)"
            # cat ./debug-and-show-tb.sh
            # echo "  gdb \"$OUT_BIN\""
        fi
    else
        echo "* skipping \"$INSTALLED_BIN\" update since it doesn't exist"
    fi
fi
echo "Done"
echo "You can now make a shortcut to:"
echo "  Exec=$OUT_BIN_PATH"
echo "  Path=$OUT_BIN_CWD"

#!/bin/sh

customExit() {
    echo
    echo "ERROR:"
    echo "$1"
    echo
    echo
    exit 1
}

DIST_DIR="`realpath dist`"
BUILD_DIR="build"
if [ ! -d "$DIST_DIR" ]; then
    customExit "ERROR: There is no $DIST_DIR directory in `pwd`"
fi
# chmod +x install.sh
echo
echo
# if [ -z "$PREFIX" ]; then PREFIX=$HOME/.local; fi
if [ -z "$PREFIX" ]; then
    PREFIX=/usr/local
    if [ "@$USER" != "@root" ]; then
        PREFIX="$HOME/.local"
        echo "* The user is not root, so the prefix became \"$PREFIX\". Specify PREFIX in the environment to override the defaults."
    fi
fi
dest_bin_dir="$PREFIX/bin"
dest_share_dir="$PREFIX/share"
project_unix_name=b3view
dest_name=$project_unix_name
src_name="$project_unix_name"
src_path="$src_name"  # detected later (dependent on how built)
dest_bin="$dest_bin_dir/$dest_name"
icon_name="b3view.png"
icon_src_path="$DIST_DIR/icons/$icon_name"
shortcut_name="org.poikilos.b3view.desktop"
shortcut_src_path="$DIST_DIR/applications/$shortcut_name"
icons_root=$dest_share_dir/pixmaps
applications_path=$dest_share_dir/applications
my_share_dir="$dest_share_dir/$project_unix_name"

mkdir -p "$my_share_dir/meshes"
mkdir -p "$my_share_dir/textures"
for resource in b3view/textures/penguin.png b3view/meshes/penguin-lowpoly-poikilos.b3d
do
    src="dist/share/$resource"
    dst="$dest_share_dir/$resource"
    cp "$src" "$dst"
    if [ $? -ne 0 ]; then
        echo "Error: 'cp \"$src\" \"$dst\"' failed."
        exit 1
    else
        echo "* installed \"$dst\""
    fi
done

mimes_path="$DIST_DIR/share/mime/packages"
USER_MIMETYPES_DB_PATH=$HOME/.local/share/mime
#USER_MIMETYPES_PATH="$USER_MIMETYPES_DB_PATH/packages"
SYSTEM_MIMETYPES_DB_PATH=/usr/share/mime
#SYSTEM_MIMETYPES_PATH=$SYSTEM_MIMETYPES_DB_PATH/packages
MIMETYPES_DB_PATH=$SYSTEM_MIMETYPES_DB_PATH

for try_build_path in  "$BUILD_DIR" "../build" "../../build-$project_unix_name-Desktop-Release/build/"
do
    try_src_path="$try_build_path/$src_name"
    if [ ! -f $src_path ]; then
        echo "Checking for $src_path...NOT FOUND"
        if [ -f "$try_src_path" ]; then
            echo "* Checking for $src_name in $try_build_path...FOUND"
            src_path="$try_src_path"
            break
        else
            echo "* Checking for $src_name in $try_build_path...NOT FOUND"
        fi
    fi
done
if [ ! -f "$src_path" ]; then
    customExit "(Nothing done) missing $src_name (You must build first, such as using build.sh or Qt Creator (Release))."
else
    echo "Checking for $src_path...FOUND"
    echo "* current directory: `pwd`"
fi

if [ -d "$dest_bin" ]; then
    customExit "(Nothing done) '$dest_bin' is a directory (should be deleted or a binary file of old version)"
fi

PROFILE_ENABLE=false
if [ -f "$dest_bin" ]; then
    rm -f "$dest_bin"
    if [ -f "$dest_bin" ]; then
        echo "WARNING: can't remove old $dest_bin, so"
        PROFILE_ENABLE=true
    fi
fi
# echo "* using '$src_path'..."
if [ ! -f "$src_path" ]; then
    customExit "* Error: missing $src_path in `pwd`"
fi
cp -f "$src_path" "$dest_bin"

if [ ! -f "$dest_bin" ]; then
    PROFILE_ENABLE=true
    prev_dir="$dest_bin_dir"
    echo "WARNING: can't write to $prev_dir, so"
fi
if [ "@$PROFILE_ENABLE" = "@true" ]; then
    dest_bin_dir="$HOME/.local/bin"
    dest_bin="$dest_bin_dir/$src_name"
    echo "installing as '$dest_bin'."
    echo "Press Ctrl C to cancel..."
    #sleep 1
    #echo "3..."
    #sleep 1
    #echo "2..."
    #sleep 1
    #echo "1..."
    #sleep 1
else
    echo "Successfully installed as '$dest_bin'"
fi

if [ "@$PROFILE_ENABLE" = "@true" ]; then
    if [ ! -d "$dest_bin_dir" ]; then mkdir "$dest_bin_dir"; fi
    if [ -L "$dest_bin" ]; then
        realpath="`readlink "$dest_bin"`"
        echo "* \"$dest_bin\" was a symlink to \"$realpath\""
        if [ ! -f "$realpath" ]; then
            echo "  * removing dangling symlink..."
            rm "$dest_bin"
        fi
    fi
    cp -f $src_path "$dest_bin" || customExit "* copying to $dest_bin_dir/ FAILED"
    icons_root="$HOME/.local/share/icons"
    applications_path="$HOME/.local/share/applications"
    MIMETYPES_DB_PATH="$USER_MIMETYPES_DB_PATH"
fi
if [ ! -d "$icons_root" ]; then
    mkdir -p "$icons_root"
fi
cp -f "$icon_src_path" "$icons_root/"
if [ $? -eq 0 ]; then
    echo "Successfully copied '$icons_root/$icon_name'"
else
    echo "FAILED to install '$icons_root/$icon_name' from `pwd`"
    exit 1
fi

if [ ! -d "$applications_path" ]; then
    mkdir -p "$applications_path"
    if [ $? -ne 0 ]; then
        echo "Error: 'mkdir -p \"$applications_path\"' failed."
        exit 1
    fi
fi

# if [ "@$PROFILE_ENABLE" = "@true" ]; then
# always rewrite, since PREFIX may differ
echo "Rewriting $applications_path/$shortcut_name..."
if [ -f "$shortcut_src_path" ]; then
    cat "$shortcut_src_path" | grep -v Exec= | grep -v Icon= > "$applications_path/$shortcut_name"
    echo "Exec=$dest_bin_dir/$project_unix_name" >> "$applications_path/$shortcut_name"
    echo "Icon=$icons_root/$icon_name" >> "$applications_path/$shortcut_name"
    echo >> "$applications_path/$shortcut_name"
else
    echo "ERROR: No icon installed since missing '$shortcut_src_path'"
fi
# else
#     cp -f "$shortcut_src_path" "$applications_path/"
# fi

if [ -f "$applications_path/$shortcut_name" ]; then
    echo "Successfully copied '$applications_path/$shortcut_name'"
else
    echo "FAILED to install '$applications_path/$shortcut_name'"
fi

if [ ! -d "$MIMETYPES_DB_PATH/packages" ]; then
    # echo "Creating '$MIMETYPES_DB_PATH/packages'..."
    mkdir "$MIMETYPES_DB_PATH/packages"
fi
update_mime_enable=false
# if [ ! -f "$mime_path" ]; then
#     echo "ERROR: Stopped installing mime types since missing $mime_path"
#     exit 1
# fi

install_mime() {
    mime_name=$1
    mime_path="$mimes_path/$mime_name"
    if [ ! -f "$mime_path" ]; then
        echo "ERROR: Stopped installing mime types since missing $mime_path"
        exit 1
    fi
    try_dest="$MIMETYPES_DB_PATH/packages/$mime_name"
    if [ ! -f "$mime_path" ]; then
        echo "* There is no source mime type at \"$mime_path\" yet."
    fi
    DEST_OP=
    if [ ! -f "$try_dest" ]; then
        # echo "* There is no destination mime type at \"$try_dest\" yet."
        DEST_OP=add
    else
        if diff -q $mime_path $try_dest; then
            echo "(You already have an identical $try_dest)"
        else
            DEST_OP=update
        fi
    fi
    if [ ! -z "$DEST_OP" ]; then
        cp -f "$mime_path" "$MIMETYPES_DB_PATH/packages/"
        if [ $? -ne 0 ]; then
            if [ -f "$MIMETYPES_DB_PATH/packages/$mime_name" ]; then
                echo "WARNING: cp failed the existing '$MIMETYPES_DB_PATH/packages/$mime_name' will be used"
                # rm -f "$MIMETYPES_DB_PATH/packages/$mime_name"
                update_mime_enable=true
            else
                echo "Error: 'cp -f \"$mime_path\" \"$MIMETYPES_DB_PATH/packages/\"' failed."
                exit 1
            fi
        fi
    fi
}

install_mime model-b3d.xml
install_mime model-x.xml
install_mime model-ms3d.xml
install_mime model-irr.xml
install_mime model-irrmesh.xml

# Since OBJ Mime type is broken on linux (detected as TGIF obj/sym
# hyperlinked vector graphics format unrelated to Wavefront OBJ
# format but sharing the same file extension), trying
# to add an overlapping mime type breaks it worse (KDE detects the
# file as "plain text file" if the xml file below is installed)
mime_name=model-obj.xml
mime_path="$mimes_path/$mime_name"
# if [ ! -f "$mime_path" ]; then
#     echo "ERROR: Stopped installing mime types since missing $mime_path"
#     exit 1
# fi
try_dest="$MIMETYPES_DB_PATH/packages/$mime_name"
# if diff -q $mime_path $try_dest; then
#    echo "* (You already have an identical $try_dest)"
# else
    # cp -f "$mime_path" "$MIMETYPES_DB_PATH/packages/"
    if [ -f "$MIMETYPES_DB_PATH/packages/$mime_name" ]; then
        # echo "* Successfully copied '$MIMETYPES_DB_PATH/packages/$mime_name'"
        echo "* Removing '$MIMETYPES_DB_PATH/packages/$mime_name' (overlaps with system usually)"
        rm -f "$MIMETYPES_DB_PATH/packages/$mime_name"
        update_mime_enable=true
    fi
# fi


if [ "@$update_mime_enable" = "@true" ]; then
    echo "Updating mime type database '$MIMETYPES_DB_PATH'..."
    update-mime-database "$MIMETYPES_DB_PATH"  # must contain packages
    if [ $? -ne 0 ]; then
        echo "Error: 'update-mime-database \"$MIMETYPES_DB_PATH\"' failed."
        exit 1
    fi

fi
echo "Done."

echo
echo

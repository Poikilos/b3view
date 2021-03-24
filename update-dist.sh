#!/bin/sh

update_file(){
    src="$1"
    dst="$2"
    op="cp"
    if [ ! -z "$3" ]; then
        op=mv
    fi
    if [ ! -f "$src" ]; then
        echo "* There is nothing to do because there is no \"$src\". See the project file there instead. To use this script, export it to png where there is no png then run this script."
        return 0
    fi
    if [ ! -f "$dst" ]; then
        echo "Error: There is no \"$dst\" to update, so for safety nothing will be done."
        return 1
    fi
    $op "$src" "$dst"
    if [ $? -eq 0 ]; then
        echo "* updated \"$dst\""
    else
        echo "Error: '$op \"$src\" \"$dst\"' failed."
    fi
}

update_file projects/textures/penguin.png dist/share/b3view/textures/penguin.png mv
update_file projects/meshes/penguin-lowpoly-poikilos.b3d dist/share/b3view/meshes/penguin-lowpoly-poikilos.b3d mv

#!/bin/sh
src=projects/textures/penguin.png
dst=../../dist/share/b3view/textures/penguin.png
if [ ! -f "$src" ]; then
    echo "* There is nothing to do because there is no \"$src\". See the xcf file there instead. To use this script, export it to png where there is no png then run this script."
    exit 0
fi
if [ ! -f "$dst" ]; then
    echo "Error: There is no \"$dst\" to update, so for safety nothing will be done."
    exit 1
fi
cp "$src" "$dst"

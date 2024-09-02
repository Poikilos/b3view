#!/bin/bash
if [ -f "`command -v dnf`" ]; then
    sudo dnf install -y irrlicht
    sudo dnf install -y irrlicht-devel gcc-c++ make
    exit $?
elif [ -f "`command -v apt`" ]; then
    sudo apt install -y libirrlicht-dev build-essential
    exit $?
else
    >&2 echo "Error: There is no apt nor dnf. You must manually install: irrlicht gcc-c++ make"
    exit 1
fi

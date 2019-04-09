#!/bin/sh

customDie() {
    echo
    echo "ERROR:"
    echo "$1"
    echo
    echo
    exit 1
}

echo "Use the install.sh in build instead. Switching to build/install.sh..."
cd build || customDie "ERROR: No build directory"
chmod +x install.sh
./install.sh

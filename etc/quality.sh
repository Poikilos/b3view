#!/bin/sh
customDie() {
    echo
    echo "ERROR:"
    echo "$1"
    echo
    echo
    exit 1
}
project_dir_name=b3view
flag_file=b3view.pro
if [ ! -f $flag_file ]; then
    if [ -f ../$flag_file ]; then
        cd ..
    fi
fi
if [ ! -f $flag_file ]; then
    echo "ERROR: There was no $flag_file in working or parent directory."
    exit 1
fi
dump_dest=/tmp/$project_dir_name
if [ -d "$dump_dest" ]; then
    rm -Rf "$dump_dest" || customDie "Cannot remove old $dump_dest"
fi
mkdir "$dump_dest" || customDie "Cannot mkdir $dump_dest"
chmod +x ./etc/pushtmp.sh
echo "* Using -style=WebKit to avoid .clang-format version issues..."
#echo "* rewriting .clang-format to avoid clang-format version issues..."
#rewriting avoids the following error:
#YAML:94:22: error: unknown key 'Delimiter'
#  - Delimiter:       pb
# clang-format -style=WebKit -dump-config > .clang-format
# echo "* writing $dump_dest using .clang-format..."
find -maxdepth 1 -name "*.cpp" -exec ./etc/pushtmp.sh {} "$dump_dest" \;
find -maxdepth 1 -name "*.h" -exec ./etc/pushtmp.sh {} "$dump_dest" \;

if [ -f "`command -v meld`" ]; then
    meld `pwd` "$dump_dest"
else
    echo "You do not have meld installed, so you'll have to diff"
    echo "against manually to see style issues, such as:"
    echo "diff -r \"`pwd`\" \"$dump_dest\""
fi
echo "Done."

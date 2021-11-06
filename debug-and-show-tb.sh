#!/bin/bash
# ^ bash for if syntax and to avoid "Bad fd number" on using $?

# This file should be identical in each project as it uses build-settings.rc. See build.sh for documentation and license.

# echo "run" > gdb.batch.txt
# echo "bt" >> gdb.batch.txt
if [ ! -f "build-settings.rc" ]; then
    echo "This file requires build-settings.rc. For documentation see build.sh."
    exit 1
fi
. build-settings.rc
if [ -z "$BIN_NAME" ]; then
    echo "BIN_NAME is required from build-settings.rc or the environment. For documentation see build.sh."
fi
MY_EXE=build/$BIN_NAME
# gdb "$MY_EXE" --command=gdb.batch.txt --batch
echo
ERR_TXT=err.txt
if [ -f "$ERR_TXT" ]; then
    rm "$ERR_TXT"
    if [ $? -ne 0 ]; then
        echo "Error: removing the old \"$ERR_TXT\" failed."
        exit 1
    fi
fi
# echo "* Output will not appear here. Check \"$ERR_TXT\" (It will be shown below when the program ends)..."
# gdb "$MY_EXE" --eval-command=run --eval-command=bt --batch  >& "$ERR_TXT"
gdb "$MY_EXE" --eval-command=run --eval-command=bt --batch  |& tee "$ERR_TXT"
# or pipe only stderr: {tmp}>&1 1>&2 2>&$tmp {tmp}>&- | tee "$ERR_TXT"
# as per https://stackoverflow.com/a/52575213/4541104
code=$?
if [ $code -ne 0 ]; then
    # echo "'gdb \"$MY_EXE\" --eval-command=run --eval-command=bt --batch  >& \"$ERR_TXT\"' failed (status=$code)."
    echo "'gdb \"$MY_EXE\" --eval-command=run --eval-command=bt --batch  |& tee \"$ERR_TXT\"' failed (status=$code)."
else
    echo "The program closed (status=$code)."
fi
# cat "$ERR_TXT"

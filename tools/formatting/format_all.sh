#!/bin/bash

root=`git rev-parse --show-toplevel`

paths=(common repair objc swift tools/templates tools/version)
formatter=$root/tools/formatting/format.sh

for path in ${paths[@]}; do
    fullpath=$root/$path
    for file in `find $fullpath \( -name "*.h" -or -name "*.hpp" -or -name "*.c" -or -name "*.cpp" -or -name "*.m" -or -name "*.mm" -or -name "*.swift" \)`; do
        if ! (sh $formatter --noadd $file); then
            exit 1
        fi
    done
done
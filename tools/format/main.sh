#!/bin/bash

root=`git rev-parse --show-toplevel`

paths=(apple common repair tools/templates)
format=$root/tools/format/code.sh

for path in ${paths[@]}; do
    fullpath=$root/$path
    for file in `find $fullpath \( -name "*.h" -or -name "*.hpp" -or -name "*.c" -or -name "*.cpp" -or -name "*.m" -or -name "*.mm" -or -name "*.swift" \)`; do
        if ! (bash "$format" --path $file); then
            exit 1
        fi
    done
done

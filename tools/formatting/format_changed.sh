#!/bin/bash

root=`git rev-parse --show-toplevel`
formatter=$root/tools/formatting/format.sh

current=`pwd`
git status -s | while read status
do
    array=($status)
    file=${array[1]}
    if [ -a $file ]; then
        fullpath=$current/$file
        sh $formatter --noadd $fullpath
    fi
done

#!/bin/bash

root=`pwd`/../../
objc=$root/objc
repair=$root/repair
common=$root/common

paths=($objc $repair $common)

for path in ${paths[@]}
do
    find $path \( -name "*.cpp" -or -name "*.c" -or -name "*.hpp" -or -name "*.h" -or -name "*.mm" -or -name "*.m" \) | xargs clang-format -i
done
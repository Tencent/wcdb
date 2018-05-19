#!/bin/bash

root=`pwd`/../../
objc=$root/objc
repair=$root/repair
common=$root/common

find $objc \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.mm" -or -name "*.m" \) | xargs clang-format -i
find $repair \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | xargs clang-format -i
find $common \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | xargs clang-format -i

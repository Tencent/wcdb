#!/bin/bash

root=`pwd`/../../
objc=$root/objc
repair=$root/repair
swift=$root/swift
xctemplate=$root/tools/templates/xctemplates

find $objc \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.mm" -or -name "*.m" \) | xargs clang-format -i
#find $repair \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | xargs clang-format -i
#find $swift \( -name "*.h" -or -name "*.c" \) | xargs clang-format -i
#if which swiftlint >/dev/null; then
#    swiftlint autocorrect --path $swift
#    swiftlint autocorrect --path $xctemplate
#else
#    echo "warning: SwiftLint not installed, download from https://github.com/realm/SwiftLint"
#fi

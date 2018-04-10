#!/bin/bash

root=`pwd`/../../
swift=$root/swift
xctemplate=$root/tools/templates/xctemplates

find $swift \( -name "*.h" -or -name "*.c" \) | xargs clang-format -i
if which swiftlint >/dev/null; then
    swiftlint autocorrect --path $swift
    swiftlint autocorrect --path $xctemplate
else
    echo "warning: SwiftLint not installed, download from https://github.com/realm/SwiftLint"
fi

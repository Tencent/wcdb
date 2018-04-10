#!/bin/bash

root=`pwd`/../../
swift=$root/swift

find $swift \( -name "*.h" -or -name "*.c" \) | xargs clang-format -i
if which swiftlint >/dev/null; then
    swiftlint autocorrect --path $swift
else
    echo "warning: SwiftLint not installed, download from https://github.com/realm/SwiftLint"
fi

#!/bin/bash

root=`pwd`/../../
xctemplate=$root/tools/templates/xctemplates

find $xctemplate \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.mm" -or -name "*.m" \) | xargs clang-format -i
if which swiftlint >/dev/null; then
    swiftlint autocorrect --path $xctemplate
else
    echo "warning: SwiftLint not installed, download from https://github.com/realm/SwiftLint"
fi

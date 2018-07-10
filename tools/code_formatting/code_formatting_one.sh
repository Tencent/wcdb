#!/bin/bash

file=${1}
if [[ -a $file ]]; then
    extension=${file##*.}
    if [ $extension = "h" ] || [ $extension = "hpp" ] || [ $extension = "c" ] || [ $extension = "cpp" ] || [ $extension = "m" ] || [ $extension = "mm" ]; then
        if command -v clang-format >/dev/null 2>&1; then
            echo "Formatting by clang-format for ${file}"
            clang-format -i $file
            git add $file
        else 
            echo "warning: clang-format not found. Install it by running 'brew install clang-format'."
            exit 1
        fi
    elif [ $extension = "swift" ]; then
        if command -v swiftlint >/dev/null 2>&1; then
            echo "Formatting by swiftlint for ${file}"
            swiftlint autocorrect --quiet --path $file
            git add $file
        else 
            echo "warning: swiftLint not found. Install it by running 'brew install swiftlint'."
            exit 1
        fi
    else 
        echo "Skip Formatting for ${file}"
    fi
else
    echo "Usage: sh code_formatting_one.sh [path]"
    exit 1
fi
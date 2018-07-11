#!/bin/bash

add=true
if [[ ${1} == "--add" ]]; then
    add=true
elif [[ ${1} == "--noadd" ]]; then
    add=false
else
    echo "Usage: sh code_formatting_one.sh [--add/--noadd] [path]"
    exit 1
fi
file=${2}
if [[ -a $file ]]; then
    extension=${file##*.}
    if [ $extension = "h" ] || [ $extension = "hpp" ] || [ $extension = "c" ] || [ $extension = "cpp" ] || [ $extension = "m" ] || [ $extension = "mm" ]; then
        if command -v clang-format >/dev/null 2>&1; then
            echo "Formatting by clang-format for ${file}"
            clang-format -i $file
            if ($add) then
                git add $file
            fi
        else 
            echo "warning: clang-format not found. Install it by running 'brew install clang-format'."
            exit 1
        fi
    elif [ $extension = "swift" ]; then
        if command -v swiftlint >/dev/null 2>&1; then
            echo "Formatting by swiftlint for ${file}"
            swiftlint autocorrect --quiet --path $file
            if ($add) then
                git add $file
            fi
        else 
            echo "warning: swiftLint not found. Install it by running 'brew install swiftlint'."
            exit 1
        fi
    else 
        echo "Skip Formatting for ${file}"
    fi
else
    echo "Usage: sh code_formatting_one.sh [--add/--noadd] [path]"
    exit 1
fi
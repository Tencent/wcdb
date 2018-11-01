#!/bin/bash

usage() {    
    echo "Usage: sh format.sh [--add/--noadd] [path]"
}

# check parameters
add=true
if [[ ${1} == "--add" ]]; then
    add=true
elif [[ ${1} == "--noadd" ]]; then
    add=false
else
    usage
    exit 1
fi
file=${2}
if [[ ! -a $file ]]; then
    usage
    exit 1    
fi

# check toolchains
if ! type clang-format > /dev/null; then
    echo "warning: clang-format not found. Install it by running 'brew install clang-format'."
    exit 1
fi
if ! type swiftlint > /dev/null; then
    echo "warning: swiftLint not found. Install it by running 'brew install swiftlint'."
    exit 1
fi

# formatting
extension=${file##*.}
formatting=false

if [ $extension = "h" ] || [ $extension = "hpp" ] || [ $extension = "c" ] || [ $extension = "cpp" ] || [ $extension = "m" ] || [ $extension = "mm" ]; then
    formatting=true
    echo "Formatting by clang-format for ${file}..."
    clang-format -i $file
    if ($add) then
        git add $file
    fi
fi

if [ $extension = "swift" ]; then
    formatting=true
    echo "Formatting by swiftlint for ${file}..."
    swiftlint autocorrect --quiet --path $file
    if ($add) then
        git add $file
    fi
fi

if [ $extension = "h" ] || [ $extension = "hpp" ] || [ $extension = "c" ] || [ $extension = "cpp" ]; then
    formatting=true
    echo "Formatting by file-macro-formatter for ${file}..."
    fileMacro=`basename $file | sed "s/[^[:alnum:]_]/_/g"`
    awk -v fileMacro=${fileMacro} '''{ 
        output=$0
        newFileMacro="_WCDB_" toupper(fileMacro)
        if ($0=="#ifndef " fileMacro) {
            output="#ifndef " newFileMacro
        }else if ($0=="#define " fileMacro) {
            output="#define " newFileMacro
        }else if ($0=="#endif \/\* " fileMacro " \*\/") {
            output="#endif /* " newFileMacro " */"
        }
        print output
    }''' $file > tmp
    mv tmp $file
fi

if ( ! $formatting) then 
    echo "Skip Formatting for ${file}"
fi
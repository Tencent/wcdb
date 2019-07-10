#!/bin/bash

usage() {    
    echo "Usage: sh $0 [-a/--add] [-p/--path path]"
}

add=false
path=""

while [[ $# -gt 0 ]]
do
key="$1"
case "$key" in
    -a|--add)
    add=true
    shift
    shift
    ;;
    -p|--path)
    path="$2"
    shift
    shift
    ;;
    *)
    echo "Unknown option: $1"
    usage
    exit 1
    ;;
esac
done

# check parameters
if [[ ! -a "$path" ]]; then
    usage
    exit 1    
fi

# check toolchains
if ! type clang-format > /dev/null; then
    echo "Error: clang-format not found. Install it by running 'brew install clang-format'."
    exit 1
fi
if ! type swiftlint > /dev/null; then
    echo "Error: swiftLint not found. Install it by running 'brew install swiftlint'."
    exit 1
fi

# formatting
extension=${path##*.}

if [ $extension = "h" ] || [ $extension = "hpp" ] || [ $extension = "c" ] || [ $extension = "cpp" ] || [ $extension = "m" ] || [ $extension = "mm" ]; then
    # clang-format
    echo "Formatting by clang-format for $path ..."
    clang-format -i "$path"
    if ($add) then
        git add "$path"
    fi
elif [ $extension = "swift" ]; then
    # swiftlint
    formatting=true
    echo "Formatting by swiftlint for $path ..."
    swiftlint autocorrect --quiet --path "$path"
    if ($add) then
        git add "$path"
    fi
else
    echo "Skip Formatting for ${path}"
fi

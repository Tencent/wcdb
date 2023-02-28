#!/bin/bash

usage() {    
    echo "Usage: bash $0 -p/--path path"
}

path=""

while [[ $# -gt 0 ]]
do
key="$1"
case "$key" in
    -p|--path)
    path="$2"
    shift
    shift
    ;;
    *)
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
elif [ $extension = "swift" ]; then
    # swiftlint
    echo "Formatting by swiftlint for $path ..."
    swiftlint --autocorrect --quiet --config ../templates/.swiftlint.yml "$path"
else
    echo "Skip Formatting for ${path}"
fi

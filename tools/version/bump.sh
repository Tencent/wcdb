#!/bin/bash

usage() {
    echo "Usage: bash $0 -v/--version version"
}

if [ ! -z "$(git status --porcelain)" ]; then 
    echo "Git working directory is not clean."
    exit 1
fi

version=""

while [[ $# -gt 0 ]]
do
key="$1"
case "$key" in
    -v|--version)
    version="$2"
    shift
    shift
    ;;
    *)
    usage
    exit 1
    ;;
esac
done

root=`git rev-parse --show-toplevel`

if [ -z "$version" -a "$version" != " " ]; then
    echo "Version is not specified."
    usage
    exit 1
fi

build=`cat ${root}/BUILD`
if [ -z "$build" -a "$build" != " " ]; then
    echo "Build is not found."
    exit 1
fi
build=$(($build+1))

echo "Bumping $version.$build"

# Root Version
echo "$version" > "$root"/VERSION
echo "$build" > "$root"/BUILD

git add "$root"/VERSION
git add "$root"/BUILD

git commit -m "Bump $version.$build"

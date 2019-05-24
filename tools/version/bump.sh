#!/bin/bash

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
    echo "Unknown option: $1"
    exit 1
    ;;
esac
done

root=`git rev-parse --show-toplevel`

if [ -z "$version" -a "$version" != " " ]; then
    echo "Version is not specified."
    exit 1
fi

build=`cat ${root}/BUILD`
if [ -z "$build" -a "$build" != " " ]; then
    echo "Build is not found."
    exit 1
fi
build=$(($build+1))

echo "Bumping" $version"."$build

# Root Version
echo "$version" > "$root"/VERSION
echo "$build" > "$root"/BUILD

# ObjC Version
template="$root"/tools/version/template.xcconfig
xcconfig="$root"/apple/support/Version.xcconfig
sed -e "s/WCDB_VERSION_PLACEHOLDER/"$version"/g" \
    -e "s/WCDB_BUILD_PLACEHOLDER/"$build"/g" \
    "$template" > "$xcconfig"
git add "$xcconfig"

git add "$root"/VERSION
git add "$root"/BUILD

gitMessage="Bump" $version"."$build
git commit -m $gitMessage
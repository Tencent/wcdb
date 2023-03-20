#!/bin/bash

usage() {
    echo "Usage: bash $0 -v/--version version"
}

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

echo "Bumping $version ..."

# Root Version
echo "$version" > "$root"/VERSION
git add "$root"/VERSION

# Xcconfig
sed -i '' "s/MARKETING_VERSION = .*/MARKETING_VERSION = $version/" "$root"/src/support/WCDB.xcconfig
git add "$root"/src/support/WCDB.xcconfig

git commit -m "Bump $version"

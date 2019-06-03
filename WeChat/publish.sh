#!/bin/bash

root=`git rev-parse --show-toplevel`

# Bump
version=`cat "$root"/VERSION`
sh "$root"/tools/version/bump.sh -v "$version"
git push origin WeChat/iOS:WeChat/iOS

wechat="$root"/WeChat
conan="$wechat"/conan
build_tool="$root"/tools/version/build.sh

# remove cache
if [ -d "$conan" ]
then
    rm -r "$conan"
fi
mkdir "$conan"

# build
if ! sh "$build_tool" --target WCDB\ iOS\ static --destination "$conan" --configuration Release; then
    exit 1
fi

# push to conan

# generate WCDB_COMMIT_ID file for recording the commit hash
version=`cat "$root"/VERSION`
build=`cat "$root"/BUILD`
gitHash=`git rev-parse HEAD`
echo $version.$build.$gitHash > "$conan"/WCDB_COMMIT_ID

# 1.0.build for current WCDB version 1.1.0.build (skipping first major version number)
version=1.0.$build

# publish to conan
sh "$wechat"/publish_to_conan.sh -p ios -d conan -n "WCDB" -v "$version"

echo "WCDB is up to ${version}"

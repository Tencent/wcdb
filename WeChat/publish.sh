#!/bin/bash

root=`git rev-parse --show-toplevel`

# Bump
version=`cat "$root"/VERSION`
sh "$root"/tools/version/bump.sh -v "$version"
git push origin WeChat/iOS:WeChat/iOS

wechat="$root"/WeChat
build="$wechat"/build
conan="$wechat"/conan
build_tool="$root"/tools/version/build.sh


# build
if [ -d "$build" ]
then
    rm -r "$build"
fi
mkdir "$build"
if ! sh "$build_tool" --platform iOS --language ObjC --disable-bitcode --static-framework --destination "$build"; then
    exit 1
fi

# push to conan
if [ -d "$conan" ]
then
    rm -r "$conan"
fi
mkdir "$conan"

mv "$build"/WCDB.framework "$conan"/WCDB.framework

# generate WCDB_COMMIT_ID file for recording the commit hash
version=`cat "$root"/VERSION`
build=`cat "$root"/BUILD`
gitHash=`git rev-parse HEAD`
echo $version.$build.$gitHash > "$conan"/WCDB_COMMIT_ID

# 1.0.build for current WCDB version 1.1.0.build (skipping first major version number)
version=1.0.$build

# publish to conan
sh "$wechat"/publish_to_conan.sh -p ios -d "$conan" -n "WCDB" -v "$version"

echo "WCDB is up to ${version}"

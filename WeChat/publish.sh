#!/bin/bash

root=`git rev-parse --show-toplevel`

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
if ! bash "$build_tool" --platform iOS --language ObjC --disable-bitcode --static-framework --destination "$build"; then
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
time=`date +"%Y%m%d.%H%M%S"`
gitHash=`git rev-parse --short HEAD`
version="$time.$gitHash"
echo $version > "$conan"/WCDB_COMMIT_ID

# publish to conan
bash "$wechat"/publish_to_conan.sh -p ios -d "$conan" -n "WCDB" -v "$version"

echo "WCDB is up to ${version}"

#!/bin/bash

root=`git rev-parse --show-toplevel`

wechat="$root"/WeChat
build="$wechat"/build
build_tool="$root"/tools/version/build.sh

# build
if [ -d "$build" ]
then
    rm -r "$build"
fi
mkdir "$build"
if ! bash "$build_tool" --platform macOS --language ObjC --configuration Release --destination "$build" --test; then
    exit 1
fi

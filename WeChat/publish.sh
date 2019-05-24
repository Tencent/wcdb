#!/bin/bash

root=`git rev-parse --show-toplevel`

# Bump
version=`cat "$root"/VERSION`
sh "$root"/tools/version/bump.sh "$version"

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
git rev-parse HEAD > "$conan"/WCDB_COMMIT_ID

# check max subversion
max_subversion=0
while read ref
do
    subversion=`echo ${ref} | sed 's/WCDB-ios\/1\.0\.\([0-9]*\)\@conan\/stable\: conan\-mmclient/\1/g'`
    if (( ${subversion} > ${max_subversion} )); then
        ((max_subversion=${subversion}))
    fi
done < <(conan remote list_ref | grep WCDB-ios/1.0.)

# 1.0.build for current WCDB version 1.1.0.build (skipping first major version number)
version=1.0.$((${max_subversion} + 1))

# publish to conan
sh "$wechat"/publish_to_conan.sh -p ios -d conan -n "WCDB" -v "$version"

echo "WCDB is up to ${version}"

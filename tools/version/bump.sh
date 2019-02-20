#!/bin/bash

root=`git rev-parse --show-toplevel`
version=`cat ${root}/VERSION`
commitHash=`git rev-parse HEAD`
buildTimestamp=`date +%s`
buildTime=`date -r ${buildTimestamp} +"%Y-%m-%d %H:%M:%S UTC%z"`

echo "Bumping" $version
echo "Hash" $commitHash
echo "Timestamp" $buildTimestamp
echo "Time" $buildTime

plists=(${root}/objc/support/WCDB_Info.plist)
for plist in ${plists[@]}; do
    echo "Bumping plist" $plist
    defaults write ${plist} CFBundleShortVersionString ${version}
    defaults write ${plist} CFBundleVersion ${version}
done

script=`git rev-parse --show-prefix`$0
identifier=`md5 -q -s com.Tencent.WCDB`

# ObjC Version
echo "Bumping ObjC Version.h" 
source=${root}/tools/version/Version.h
target=${root}/objc/source/utility/Version.h

sed -e "s/WCDB_IDENTIFIER_PLACEHOLDER/${identifier}/g" \
    -e "s/WCDB_VERSION_PLACEHOLDER/${version}/g" \
    -e "s/WCDB_COMMIT_HASH_PLACEHOLDER/${commitHash}/g" \
    -e "s/WCDB_BUILD_TIMESTAMP_PLACEHOLDER/${buildTimestamp}/g" \
    -e "s/WCDB_BUILD_TIME_PLACEHOLDER/${buildTime}/g" \
    -e "s/WCDB_VERSION_TOOL_PLACEHOLDER/${script////\\/}/g" \
    ${source} > ${target}

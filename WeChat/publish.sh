#!/bin/bash

root=`git rev-parse --show-toplevel`
project=${root}/objc/WCDB.xcodeproj
scheme=WCDB\ iOS\ Static
configuration=Release
iphoneosSDK=iphoneos
iphonesimulatorSDK=iphonesimulator
parameters=ONLY_ACTIVE_ARCH=NO CODE_SIGNING_REQUIRED=NO CODE_SIGN_IDENTITY= SKIP_INSTALL=YES GCC_INSTRUMENT_PROGRAM_FLOW_ARCS=NO CLANG_ENABLE_CODE_COVERAGE=NO STRIP_INSTALLED_PRODUCT=NO
framework=WCDB.framework
binary=WCDB

pathToWeChat=${root}/WeChat
pathToDerivedData=${pathToWeChat}/DerivedData
pathToProducts=${pathToDerivedData}/Build/Products
pathToDeviceFramework=${pathToProducts}/${configuration}-${iphoneosSDK}/${framework}
pathToDeviceBinary=${pathToDeviceFramework}/${binary}
pathToSimulatorBinary=${pathToProducts}/${configuration}-${iphonesimulatorSDK}/${framework}/${binary}

pathToFinalFramework=${pathToDerivedData}/${framework}
pathToFinalBinary=${pathToFinalFramework}/${binary}

# remove cache
if [ -d ${pathToDerivedData} ]
then
    rm -r ${pathToDerivedData}
fi

# build device
if ! xcrun xcodebuild -arch arm64 -project ${project} -scheme "${scheme}" -configuration ${configuration} -derivedDataPath ${pathToDerivedData} -sdk ${iphoneosSDK} ${parameters} build; then
    echo "Build Failed for Device."
    exit 1
fi

# build simulator
if ! xcrun xcodebuild -project ${project} -scheme "${scheme}" -configuration ${configuration} -derivedDataPath ${pathToDerivedData} -sdk ${iphonesimulatorSDK} ${parameters} build -destination platform=iOS\ Simulator,name="iPhone X"; then
    echo "Build Failed for Simulator."
    exit 1
fi

# copy a template(headers) to the target path
cp -R ${pathToDeviceFramework} ${pathToFinalFramework}

# combine both two binaries
if ! xcrun lipo -create ${pathToDeviceBinary} ${pathToSimulatorBinary} -output ${pathToFinalBinary}; then
    echo "Lipo Failed for Simulator."
    exit 1
fi

# push to conan
pathToConan=${pathToWeChat}/conan
pathToCommitID=${pathToConan}/WCDB_COMMIT_ID
pathToPublishConanScript=${pathToWeChat}/publish_to_conan.sh
pathToFrameworkForConan=${pathToConan}/${framework}

# remove cache
if [ -d ${pathToConan} ]
then
    rm -r ${pathToConan}
fi
mkdir ${pathToConan}

# copy universal framework to conan
cp -R ${pathToFinalFramework} ${pathToFrameworkForConan}

# generate WCDB_COMMIT_ID file for recording the commit hash
git rev-parse HEAD > ${pathToCommitID}

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
sh ${pathToPublishConanScript} -p ios -d conan -n "WCDB" -v ${version}

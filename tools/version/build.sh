#!/bin/bash

usage() {
    echo """USAGE
  sh $0 
       -t/--target WCDB/WCDB iOS static/WCDBSwift 
       [-c/--configuration Debug/Release] 
       [-d/--destination destination]
       [--disable-bitcode]
"""
}

root=`git rev-parse --show-toplevel`

target="" # WCDB, WCDB iOS static, WCDBSwift
configuration="Release" # Release, Debug
destination="./"
disable_bitcode=false

while [[ $# -gt 0 ]]
do
key="$1"
case "$key" in
    -t|--target)
    target="$2"
    shift
    shift
    ;;
    -c|--configuration)
    configuration="$2"
    shift
    shift
    ;;
    -d|--destination)
    destination="$2"
    shift
    shift
    ;;
    --disable-bitcode)
    disable_bitcode=true
    shift
    ;;
    *)
    usage
    exit 1
    ;;
esac
done

case "$target" in
    WCDB|WCDB\ iOS\ static|WCDBSwift)
    ;;
    *)
    usage
    exit 1
    ;;
esac

case "$configuration" in
    Debug|Release)
    ;;
    *)
    usage
    exit 1
    ;;
esac

project="$root"/apple/WCDB.xcodeproj
derivedData="$root"/apple/derivedData
products="$derivedData"/Build/Products

deviceFramework="$products"/"$configuration"-iphoneos/WCDB.framework
simulatorFramework="$products"/"$configuration"-iphonesimulator/WCDB.framework
destination="$destination"/WCDB.framework

parameters=(ONLY_ACTIVE_ARCH=NO CODE_SIGNING_REQUIRED=NO CODE_SIGN_IDENTITY= SKIP_INSTALL=YES GCC_INSTRUMENT_PROGRAM_FLOW_ARCS=NO CLANG_ENABLE_CODE_COVERAGE=NO ENABLE_TESTABILITY=NO)
if [ "$disable_bitcode" = true ] ; then
    parameters+=(ENABLE_BITCODE=NO)
fi

echo "Build $target with $configuration to $destination"

# remove cache
if [ -d "$derivedData" ]
then
    rm -r "$derivedData"
fi
if [ -d "$destination" ]
then
    rm -r "$destination"
fi

# build device
if ! xcrun xcodebuild -arch arm64 -scheme "$target" -project "$project" -configuration "$configuration" -derivedDataPath "$derivedData" -sdk iphoneos ${parameters[@]} build; then
    echo "Build failed for device."
    exit 1
fi

# build simulator
if ! xcrun xcodebuild -arch x86_64 -scheme "$target" -project "$project" -configuration "$configuration" -derivedDataPath "$derivedData" -sdk iphonesimulator ${parameters[@]} build; then
    echo "Build failed for simulator."
    exit 1
fi

# copy template headers to the destination
if ! cp -R "$deviceFramework" "$destination"; then
    echo "Copy header failed."
    exit 1
fi

# combine binaries
if ! xcrun lipo -create "$deviceFramework"/WCDB "$simulatorFramework"/WCDB -output "$destination"/WCDB; then
    echo "Lipo failed."
    exit 1
fi

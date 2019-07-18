#!/bin/bash

showUsage() {
    echo """USAGE
  sh $0 
       -p/--platform iOS/macOS
       -l/--language ObjC/Swift
       [-d/--destination destination]
       [--disable-bitcode] 
       [--static-framework]
"""
}

root=`git rev-parse --show-toplevel`

platform=""
language=""
destination="./"
contains_32bit=false
disable_bitcode=false
static_framework=false

while [[ $# -gt 0 ]]
do
key="$1"
case "$key" in
    -l|--language)
    language="$2"
    shift
    shift
    ;;
    -p|--platform)
    platform="$2"
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
    --static-framework)
    static_framework=true
    shift
    ;;
    -h|--help)
    showUsage
    exit 0
    ;;
    *)
    showUsage
    exit 1
    ;;
esac
done

project="$root"/apple/WCDB.xcodeproj
derivedData="$destination"/derivedData
products="$derivedData"/Build/Products
configuration=Release
parameters=(ONLY_ACTIVE_ARCH=NO CODE_SIGNING_REQUIRED=NO CODE_SIGN_IDENTITY= SKIP_INSTALL=YES GCC_INSTRUMENT_PROGRAM_FLOW_ARCS=NO CLANG_ENABLE_CODE_COVERAGE=NO ENABLE_TESTABILITY=NO)
if $disable_bitcode; then
    parameters+=(ENABLE_BITCODE=NO)
fi
target=""
productName=""
product=""

case "$language" in
    ObjC)
        if $static_framework; then
            target="WCDB iOS static"
        else
            target="WCDB"
        fi
        productName="WCDB.framework"
    ;;
    Swift)
        if $static_framework; then
            echo 'Static library is only support iOS + ObjC.'
            exit 1
        else
            target="WCDBSwift"
        fi
        productName="WCDBSwift.framework"
    ;;
    *)
        echo 'Language should be either ObjC or Swift.'
        showUsage
        exit 1
    ;;
esac

# build
if [ -d "$derivedData" ]
then
    rm -r "$derivedData"
fi
case "$platform" in
    iOS)
        product="$products"/"$configuration"-iphoneos/"$productName"
        productSimulator="$products"/"$configuration"-iphonesimulator/"$productName"
        if ! xcrun xcodebuild -arch arm64 -scheme "$target" -project "$project" -configuration "$configuration" -derivedDataPath "$derivedData" -sdk iphoneos ${parameters[@]} build; then
            exit 1
        fi
        if ! xcrun xcodebuild -arch x86_64 -scheme "$target" -project "$project" -configuration "$configuration" -derivedDataPath "$derivedData" -sdk iphonesimulator ${parameters[@]} build; then
            exit 1
        fi
        if ! xcrun lipo -create "$product"/WCDB "$productSimulator"/WCDB -output "$product"/WCDB; then
            exit 1
        fi
    ;;
    macOS)
        product="$products"/"$configuration"/"$productName"
        if ! xcrun xcodebuild -arch x86_64 -scheme "$target" -project "$project" -configuration "$configuration" -derivedDataPath "$derivedData" -sdk macosx ${parameters[@]} build; then
            exit 1
        fi
    ;;
    *)
        echo 'Platform should be either iOS or macOS.'
        showUsage
        exit 1
    ;;
esac

# copy to destination
output="$destination"/"$productName"
if [ -d "$output" ]
then
    rm -r "$output"
fi
if ! cp -R "$product" "$destination"; then
    echo "Copy header failed."
    exit 1
fi

#!/bin/bash

showUsage() {
    echo """\
  USAGE: bash $0 
       -p/--platform iOS/macOS/watchOS
       -l/--language All/ObjC/Swift/Cpp
       [-c/--configuration Debug/Release]: Default to Release.
       [-d/--destination destination]: Default to current directory.
       [--test]: Enable test.
       [--disable-bitcode]: Use bitcode if not specified.
       [--static-framework]: Produce dynamic framework if not specified.
       [--pretty]: Use \`xcpretty\` if available.
       [--wechat]: For WeChat.
       [--universal]: Build universal library for macOS.
"""
}

ScriptDir=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
root="$ScriptDir"/../../

declare platform
declare language
destination="."
enable_bitcode=true
static_framework=false
configuration=Release
pretty=false
action=build
wechat=false
universal=false

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
    --test)
    action=test
    shift
    shift
    ;;
    -c|--configuration)
    configuration="$2"
    shift
    shift
    ;;
    --disable-bitcode)
    enable_bitcode=false
    shift
    ;;
    --static-framework)
    static_framework=true
    shift
    ;;
    --pretty)
    pretty=true
    shift
    ;;
    --wechat)
    wechat=true
    shift
    ;;
    --universal)
    universal=true
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

project="$root"/src/WCDB.xcodeproj
derivedData="$destination"/derivedData
products="$derivedData"/Build/Products

if $universal; then
    if [ "$platform" != "macOS" ]; then
        echo 'Universal library is only supported with macOS.'
        exit 1
    fi
fi

declare settingsPrameter
if $static_framework; then
    settingsPrameter+="--static-framework "
fi
if $enable_bitcode; then
	settingsPrameter+="--enable-bitcode "
fi
if $wechat; then
	settingsPrameter+="--wechat "
fi

settings=`bash $ScriptDir/settings.sh $settingsPrameter`

declare target
declare scheme
case "$language" in
    All)
        target=WCDB
        scheme=WCDB
    ;;
    ObjC)
        target=WCDBObjc
        scheme=WCDBObjc
    ;;
    Swift)
        target=WCDBSwift
        scheme=WCDBSwift
    ;;
    Cpp)
        target=WCDBCpp
        scheme=WCDBCpp
    ;;
    *)
        echo 'Language should be All/ObjC/Swift/Cpp.'
        showUsage
        exit 1
    ;;
esac
platformBasedParameters=()
case "$platform" in
    iOS)
        platformBasedParameters+=('product="$products/$configuration-iphoneos/$target.framework" sdk=iphoneos arch="arm64"')
        platformBasedParameters+=('product="$products/$configuration-iphonesimulator/$target.framework" sdk=iphonesimulator arch=x86_64')
    ;;
    macOS)
        macosArchs='x86_64'
        if $universal; then
        	macosArchs='x86_64 -arch arm64'
        fi
        platformBasedParameters+=('product="$products/$configuration/$target.framework" sdk=macosx arch=$macosArchs')
    ;;
    watchOS)
        platformBasedParameters+=('product="$products/$configuration-watchos/$target.framework" sdk=watchos arch="armv7k -arch arm64_32"')
        platformBasedParameters+=('product="$products/$configuration-watchsimulator/$target.framework" sdk=watchsimulator arch=x86_64')
    ;;
    *)
        echo 'Platform should be either iOS, macOS or watchOS.'
        showUsage
        exit 1
    ;;
esac

# build
echo "Building $version ..."
echo "    Action: ${action}"
echo "    Language: ${language}"
echo "    Platform: ${platform}"
echo "    Configuration: ${configuration}"
echo "    Destination: ${destination}"
echo "    Bitcode: ${enable_bitcode}"
echo "    Static: ${static_framework}"
echo "    Pretty: ${pretty}"
echo ""

if [ -d "$derivedData" ]
then
    rm -r "$derivedData"
fi
declare template
machos=()
productsDir=()
for platformBasedParameter in "${platformBasedParameters[@]}"; do
    eval "$platformBasedParameter"
    machos+=( "$product/$target" )
    productsDir+=( $product )
    if [ -z "$template" ]; then
        template="$product"
    fi
    builder="xcrun xcodebuild -arch $arch -scheme $scheme -project $project -configuration $configuration -derivedDataPath $derivedData -sdk $sdk $settings $action"
    if $pretty; then
        if type xcpretty > /dev/null; then
            builder+=" | xcpretty"
        fi
    fi
    if ! eval "$builder"; then
        echo "Build failed."
        exit 1
    fi
done

# copy to destination
output="$destination"/"$target.framework"
if [ -d "$output" ]
then
    rm -r "$output"
fi
if ! cp -R "$template" "$destination"; then
    echo "Copy frameworks failed."
    exit 1
fi

swiftHeader="$output"/"Headers/$target-Swift.h"
if [ `grep -c "unsupported Swift architecture" $swiftHeader` -ne '0' ];then
    lineNum=$(sed -n '$=' $swiftHeader)
    sed -n '3,'$((lineNum-3))'p' $swiftHeader | sed -n 'w'$swiftHeader
    echo "Strip swift header $swiftHeader"
fi

if (( ${#machos[@]} > 1 )); then
    if ! xcrun lipo -create "${machos[@]}" -output "$output/$target"; then
        echo "Lipo mach-o failed."
        exit 1
    fi
    for product in "${productsDir[@]}"; do
        if ! cp -Rf "$product/Modules" "$output"; then
            echo "Copy Modules $product/Modules failed."
            exit 1
        fi
    done
fi
echo "Output at $output"

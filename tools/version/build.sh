#!/bin/bash

showUsage() {
    echo """\
  USAGE: bash $0 
       -p/--platform iOS/macOS
       -l/--language ObjC/Swift
       [-c/--configuration Debug/Release]: Default to Release.
       [-d/--destination destination]: Default to current directory.
       [--test]: Enable test.
       [--disable-bitcode]: Use bitcode if not specified.
       [--static-framework]: Produce dynamic framework if not specified.
       [--pretty]: Use \`xcpretty\` if available.
"""
}

root=`git rev-parse --show-toplevel`

declare platform
declare language
destination="."
contains_32bit=false
enable_bitcode=true
static_framework=false
configuration=Release
pretty=false
action=build

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

# version
timestamp=`date +"%Y-%m-%d %H:%M:%S UTC%z"`
version=`cat $root/VERSION`
build=`cat $root/BUILD`

project="$root"/apple/WCDB.xcodeproj
derivedData="$destination"/derivedData
products="$derivedData"/Build/Products

preprocessor="\$\(inherited\) WCDB_TIMESTAMP=\"$timestamp\" WCDB_VERSION=\"$version\" WCDB_BUILD=\"$build\""
settings=(ONLY_ACTIVE_ARCH=NO CODE_SIGNING_REQUIRED=NO CODE_SIGN_IDENTITY= SKIP_INSTALL=YES GCC_INSTRUMENT_PROGRAM_FLOW_ARCS=NO CLANG_ENABLE_CODE_COVERAGE=NO ENABLE_TESTABILITY=NO GCC_PREPROCESSOR_DEFINITIONS=${preprocessor})

if $static_framework; then
    if [ "$language" != "ObjC" ] || [ "$platform" != "iOS" ]; then
        echo 'Static library is only support iOS + ObjC.'
        exit 1
    fi
    settings+=(MACH_O_TYPE=staticlib STRIP_STYLE=debugging LLVM_LTO=NO STRIP_INSTALLED_PRODUCT=NO DEPLOYMENT_POSTPROCESSING=NO)
fi
if ! $enable_bitcode; then
    settings+=(ENABLE_BITCODE=NO)
fi

declare target
case "$language" in
    ObjC)
        target=WCDB
    ;;
    Swift)
        target=WCDBSwift
    ;;
    *)
        echo 'Language should be either ObjC or Swift.'
        showUsage
        exit 1
    ;;
esac
platformBasedParameters=()
case "$platform" in
    iOS)
        platformBasedParameters+=('product="$products/$configuration-iphoneos/$target.framework" sdk=iphoneos arch=arm64')
        platformBasedParameters+=('product="$products/$configuration-iphonesimulator/$target.framework" sdk=iphonesimulator arch=x86_64')
    ;;
    macOS)
        platformBasedParameters+=('product="$products/$configuration/$target.framework" sdk=macosx arch=x86_64')
    ;;
    *)
        echo 'Platform should be either iOS or macOS.'
        showUsage
        exit 1
    ;;
esac

# build
log="Building $version/$build ..."

echo "Building $version/$build ..."
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
for platformBasedParameter in "${platformBasedParameters[@]}"; do
    eval "$platformBasedParameter"
    machos+=( "$product/$target" )
    if [ -z "$template" ]; then
        template="$product"
    fi
    builder="xcrun xcodebuild -arch "$arch" -scheme "$target" -project "$project" -configuration "$configuration" -derivedDataPath "$derivedData" -sdk "$sdk" ${settings[@]} ${action}"
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
if (( ${#machos[@]} > 1 )); then
    if ! xcrun lipo -create "${machos[@]}" -output "$output/$target"; then
        echo "Lipo mach-o failed."
        exit 1
    fi
fi
echo "Output at $output"

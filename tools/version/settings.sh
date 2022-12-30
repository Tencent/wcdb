#!/bin/bash

showUsage() {
    echo """\
  USAGE: bash $0 
       [--enable-bitcode]: Disable bitcode if not specified.
       [--static-framework]: Produce dynamic framework if not specified.
       [--wechat]: For WeChat.
"""
}
wechat=false
enable_bitcode=false
static_framework=false

while [[ $# -gt 0 ]]
do
key="$1"
case "$key" in
    --enable-bitcode)
    enable_bitcode=true
    shift
    ;;
    --static-framework)
    static_framework=true
    shift
    ;;
    --wechat)
    wechat=true
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

ScriptDir=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
root="$ScriptDir"/../../
# version
timestamp=`date +"%Y-%m-%d %H:%M:%S UTC%z"`
version=`cat $root/VERSION`

preprocessor="\$\(inherited\)"
preprocessor+="\ WCDB_VERSION=\\\"\"${version}\\\"\""
if $wechat; then
    preprocessor+="\ WCDB_WECHAT=1"
fi
preprocessor+="\ WCDB_BUILD=\\\"\"${timestamp}\\\"\""

settings=(ONLY_ACTIVE_ARCH=NO CODE_SIGNING_REQUIRED=NO CODE_SIGN_IDENTITY= SKIP_INSTALL=YES GCC_INSTRUMENT_PROGRAM_FLOW_ARCS=NO CLANG_ENABLE_CODE_COVERAGE=NO ENABLE_TESTABILITY=NO GCC_PREPROCESSOR_DEFINITIONS=${preprocessor})
if $wechat; then
    settings+=(DEPLOYMENT_POSTPROCESSING=NO)
fi
if $static_framework; then
    settings+=(MACH_O_TYPE=staticlib STRIP_STYLE=debugging LLVM_LTO=NO STRIP_INSTALLED_PRODUCT=NO DEPLOYMENT_POSTPROCESSING=NO)
fi
if ! $enable_bitcode; then
    settings+=(ENABLE_BITCODE=NO)
fi
echo ${settings[@]}

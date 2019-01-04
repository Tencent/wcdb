#!/bin/bash

root=`git rev-parse --show-toplevel`
version=`cat ${root}/VERSION`
for plist in `find ${root}/objc/support -name '*.plist'`; do
    echo ${plist} ${version}
    defaults write ${plist} CFBundleShortVersionString ${version}
    defaults write ${plist} CFBundleVersion ${version}
done

sh objc.sh

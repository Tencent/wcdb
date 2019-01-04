#!/bin/bash

root=`git rev-parse --show-toplevel`
version=`cat ${root}/VERSION`
defaults write ${root}/objc/support/WCDB_Info.plist CFBundleShortVersionString ${version}
defaults write ${root}/objc/support/TestCase_Info.plist CFBundleShortVersionString ${version}
sh objc.sh

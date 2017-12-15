#!/bin/bash

appledoc objc/settings.plist ../../objc/WCDB
#jazzy \
#    --objc \
#    --clean \
#    --author sanhuazhang \
#    --author_url sanhuazhang@tencent.com \
#    --github_url https://github.com/Tencent/wcdb \
#    --xcodebuild-arguments --objc,WCDB/WCDB.h,--,-x,objective-c++,-isysroot,`xcrun --show-sdk-path --sdk iphoneos` \
#    --root-url https://github.com/Tencent/wcdb \
#    --source-directory ../../objc/ \
#    --skip-undocumented \
#    --copyright "Copyright © 2017 Tencent. All rights reserved." \
#    --output objc/
    
jazzy \
    --no-objc \
    --clean \
    --author sanhuazhang \
    --author_url sanhuazhang@tencent.com \
    --github_url https://github.com/Tencent/wcdb \
    --xcodebuild-arguments -scheme,WCDB \
    --module WCDBSwift \
    --root-url https://github.com/Tencent/wcdb \
    --source-directory ../../swift/ \
    --skip-undocumented \
    --copyright "Copyright © 2017 Tencent. All rights reserved." \
    --output swift/

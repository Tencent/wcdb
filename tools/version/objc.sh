#!/bin/bash

root=`git rev-parse --show-toplevel`
buildTimestamp=`date +%s`
buildDate=`date -r ${buildTimestamp} +"%Y-%m-%d %H:%M:%S UTC%z"`
commitID=`git rev-parse HEAD`
version=`defaults read /Users/sanhuazhang/Developer/Github/wcdb/objc/Info.plist CFBundleShortVersionString`

echo """
/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WCDBVersion_h
#define WCDBVersion_h

/* DO NOT CHANGE IT MANUALLY */

#define WCDB_VERSION \"${version}\"
#define WCDB_COMMIT_ID \"${commitID}\"
#define WCDB_BUILD_TIMESTAMP ${buildTimestamp}
#define WCDB_BUILD_TIME \"${buildDate}\"

#endif /* WCDBVersion_h */

""" > ${root}/objc/source/WCDBVersion.h
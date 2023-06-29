//
// Created by qiuwenchen on 2023/6/29.
//

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

#import "AppAttach.h"
#import <Foundation/Foundation.h>

@implementation AppAttach

WCDB_IMPLEMENTATION(AppAttach)

WCDB_SYNTHESIZE(appId);
WCDB_SYNTHESIZE(sdkVer);
WCDB_SYNTHESIZE(mediaSvrId);
WCDB_SYNTHESIZE(mediaId);
WCDB_SYNTHESIZE(clientAppDataId);
WCDB_SYNTHESIZE(type);
WCDB_SYNTHESIZE(totalLen);
WCDB_SYNTHESIZE(offset);
WCDB_SYNTHESIZE(status);
WCDB_SYNTHESIZE(isUpload);
WCDB_SYNTHESIZE(createTime);
WCDB_SYNTHESIZE(lastModifyTime);
WCDB_SYNTHESIZE(fileFullPath);
WCDB_SYNTHESIZE(fullXml);
WCDB_SYNTHESIZE(msgInfoId);
WCDB_SYNTHESIZE(netTimes);
WCDB_SYNTHESIZE(isUseCdn);
WCDB_SYNTHESIZE(signature);
WCDB_SYNTHESIZE(fakeAeskey);
WCDB_SYNTHESIZE(fakeSignature);

WCDB_INDEX("_status", status);
WCDB_INDEX("_msgInfoId", msgInfoId);

@end

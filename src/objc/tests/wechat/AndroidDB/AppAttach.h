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

#import <WCDB/WCDB.h>

@interface AppAttach : NSObject <WCTTableCoding>

@property (nonatomic, strong) NSString* appId;
@property (nonatomic, assign) SInt64 sdkVer;
@property (nonatomic, strong) NSString* mediaSvrId;
@property (nonatomic, strong) NSString* mediaId;
@property (nonatomic, strong) NSString* clientAppDataId;
@property (nonatomic, assign) SInt64 type;
@property (nonatomic, assign) SInt64 totalLen;
@property (nonatomic, assign) SInt64 offset;
@property (nonatomic, assign) SInt64 status;
@property (nonatomic, assign) SInt64 isUpload;
@property (nonatomic, assign) SInt64 createTime;
@property (nonatomic, assign) SInt64 lastModifyTime;
@property (nonatomic, strong) NSString* fileFullPath;
@property (nonatomic, strong) NSString* fullXml;
@property (nonatomic, assign) SInt64 msgInfoId;
@property (nonatomic, assign) SInt64 netTimes;
@property (nonatomic, assign) SInt64 isUseCdn;
@property (nonatomic, strong) NSString* signature;
@property (nonatomic, strong) NSString* fakeAeskey;
@property (nonatomic, strong) NSString* fakeSignature;

WCDB_PROPERTY(appId);
WCDB_PROPERTY(sdkVer);
WCDB_PROPERTY(mediaSvrId);
WCDB_PROPERTY(mediaId);
WCDB_PROPERTY(clientAppDataId);
WCDB_PROPERTY(type);
WCDB_PROPERTY(totalLen);
WCDB_PROPERTY(offset);
WCDB_PROPERTY(status);
WCDB_PROPERTY(isUpload);
WCDB_PROPERTY(createTime);
WCDB_PROPERTY(lastModifyTime);
WCDB_PROPERTY(fileFullPath);
WCDB_PROPERTY(fullXml);
WCDB_PROPERTY(msgInfoId);
WCDB_PROPERTY(netTimes);
WCDB_PROPERTY(isUseCdn);
WCDB_PROPERTY(signature);
WCDB_PROPERTY(fakeAeskey);
WCDB_PROPERTY(fakeSignature);

@end

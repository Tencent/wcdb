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

@interface ImgInfo : NSObject <WCTTableCoding>

@property (nonatomic, assign) SInt64 msgId;
@property (nonatomic, assign) SInt64 msgSvrId;
@property (nonatomic, assign) SInt32 offset;
@property (nonatomic, assign) SInt32 totalLen;
@property (nonatomic, strong) NSString* bigImgPath;
@property (nonatomic, strong) NSString* thumbImgPath;
@property (nonatomic, assign) SInt32 createtime;
@property (nonatomic, assign) SInt32 msglocalid;
@property (nonatomic, assign) SInt32 status;
@property (nonatomic, assign) SInt32 nettimes;
@property (nonatomic, assign) SInt32 reserved1;
@property (nonatomic, assign) SInt32 reserved2;
@property (nonatomic, strong) NSString* reserved3;
@property (nonatomic, strong) NSString* reserved4;
@property (nonatomic, assign) SInt32 hashdthumb;
@property (nonatomic, assign) SInt32 iscomplete;
@property (nonatomic, strong) NSString* origImgMD5;
@property (nonatomic, assign) SInt32 compressType;
@property (nonatomic, strong) NSString* midImgPath;
@property (nonatomic, assign) SInt32 forwardType;
@property (nonatomic, strong) NSString* hevcPath;
@property (nonatomic, assign) SInt32 sendImgType;

WCDB_PROPERTY(msgId);
WCDB_PROPERTY(msgSvrId);
WCDB_PROPERTY(offset);
WCDB_PROPERTY(totalLen);
WCDB_PROPERTY(bigImgPath);
WCDB_PROPERTY(thumbImgPath);
WCDB_PROPERTY(createtime);
WCDB_PROPERTY(msglocalid);
WCDB_PROPERTY(status);
WCDB_PROPERTY(nettimes);
WCDB_PROPERTY(reserved1);
WCDB_PROPERTY(reserved2);
WCDB_PROPERTY(reserved3);
WCDB_PROPERTY(reserved4);
WCDB_PROPERTY(hashdthumb);
WCDB_PROPERTY(iscomplete);
WCDB_PROPERTY(origImgMD5);
WCDB_PROPERTY(compressType);
WCDB_PROPERTY(midImgPath);
WCDB_PROPERTY(forwardType);
WCDB_PROPERTY(hevcPath);
WCDB_PROPERTY(sendImgType);

@end

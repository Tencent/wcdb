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

#import "ImgInfo.h"

@implementation ImgInfo

WCDB_IMPLEMENTATION(ImgInfo)

WCDB_SYNTHESIZE(msgId);
WCDB_SYNTHESIZE(msgSvrId);
WCDB_SYNTHESIZE(offset);
WCDB_SYNTHESIZE(totalLen);
WCDB_SYNTHESIZE(bigImgPath);
WCDB_SYNTHESIZE(thumbImgPath);
WCDB_SYNTHESIZE(createtime);
WCDB_SYNTHESIZE(msglocalid);
WCDB_SYNTHESIZE(status);
WCDB_SYNTHESIZE(nettimes);
WCDB_SYNTHESIZE(reserved1);
WCDB_SYNTHESIZE(reserved2);
WCDB_SYNTHESIZE(reserved3);
WCDB_SYNTHESIZE(reserved4);
WCDB_SYNTHESIZE(hashdthumb);
WCDB_SYNTHESIZE(iscomplete);
WCDB_SYNTHESIZE(origImgMD5);
WCDB_SYNTHESIZE(compressType);
WCDB_SYNTHESIZE(midImgPath);
WCDB_SYNTHESIZE(forwardType);
WCDB_SYNTHESIZE(hevcPath);
WCDB_SYNTHESIZE(sendImgType);

WCDB_PRIMARY(msgId);

WCDB_INDEX("_msgSvrId", msgSvrId);
WCDB_INDEX("_msglocalid", msglocalid);
WCDB_INDEX("_reserved1", reserved1);
WCDB_DEFAULT(hashdthumb, 0);
WCDB_DEFAULT(iscomplete, 1);
WCDB_INDEX("_iscomplete", iscomplete);
WCDB_INDEX("_origImgMD5", origImgMD5);
WCDB_DEFAULT(compressType, 0);
WCDB_DEFAULT(forwardType, 0);
WCDB_DEFAULT(sendImgType, 0);

@end

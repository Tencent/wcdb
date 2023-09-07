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

#import "Message.h"
#import <Foundation/Foundation.h>

@implementation Message

@synthesize lastInsertedRowID;
@synthesize isAutoIncrement;

WCDB_IMPLEMENTATION(Message)
WCDB_SYNTHESIZE(msgId);
WCDB_SYNTHESIZE(msgSvrId);
WCDB_SYNTHESIZE(type);
WCDB_SYNTHESIZE(status);
WCDB_SYNTHESIZE(isSend);
WCDB_SYNTHESIZE(isShowTimer);
WCDB_SYNTHESIZE(createTime);
WCDB_SYNTHESIZE(content);
WCDB_SYNTHESIZE(imgPath);
WCDB_SYNTHESIZE(reserved);
WCDB_SYNTHESIZE(lvBuffer);
WCDB_SYNTHESIZE(transContent);
WCDB_SYNTHESIZE(transBrandWording);
WCDB_SYNTHESIZE(bizClientMsgId);
WCDB_SYNTHESIZE(bizChatId);
WCDB_SYNTHESIZE(bizChatUserId);
WCDB_SYNTHESIZE(msgSeq);
WCDB_SYNTHESIZE(flag);
WCDB_SYNTHESIZE(solitaireFoldInfo);
WCDB_SYNTHESIZE(historyId);

WCDB_PRIMARY(msgId);

WCDB_INDEX("_msgSvrId", msgSvrId);
WCDB_INDEX("_createTime", createTime);
WCDB_INDEX("_msgSeq", msgSeq);
WCDB_DEFAULT(bizChatId, -1);

WCDB_INDEX("_status_isSend_createTime", status);
WCDB_INDEX("_status_isSend_createTime", isSend);
WCDB_INDEX("_status_isSend_createTime", createTime);

WCDB_INDEX("_type_createTime", type);
WCDB_INDEX("_type_createTime", createTime);

WCDB_INDEX("_isSend_createTime", isSend);
WCDB_INDEX("_isSend_createTime", createTime);

WCDB_INDEX("_flag_msgSeq", flag);
WCDB_INDEX("_flag_msgSeq", msgSeq);

@end

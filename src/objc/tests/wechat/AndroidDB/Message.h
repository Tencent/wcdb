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

#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@interface Message : NSObject <WCTTableCoding>

@property (nonatomic, assign) SInt64 msgId;
@property (nonatomic, assign) SInt64 msgSvrId;
@property (nonatomic, assign) SInt32 type;
@property (nonatomic, assign) SInt32 status;
@property (nonatomic, assign) SInt32 isSend;
@property (nonatomic, assign) SInt32 isShowTimer;
@property (nonatomic, assign) SInt64 createTime;
@property (nonatomic, strong) NSString* content;
@property (nonatomic, strong) NSString* imgPath;
@property (nonatomic, strong) NSString* reserved;
@property (nonatomic, strong) NSData* lvBuffer;
@property (nonatomic, strong) NSString* transContent;
@property (nonatomic, strong) NSString* transBrandWording;
@property (nonatomic, strong) NSString* bizClientMsgId;
@property (nonatomic, assign) SInt64 bizChatId;
@property (nonatomic, strong) NSString* bizChatUserId;
@property (nonatomic, assign) SInt64 msgSeq;
@property (nonatomic, assign) SInt32 flag;
@property (nonatomic, strong) NSData* solitaireFoldInfo;
@property (nonatomic, strong) NSString* historyId;

WCDB_PROPERTY(msgId);
WCDB_PROPERTY(msgSvrId);
WCDB_PROPERTY(type);
WCDB_PROPERTY(status);
WCDB_PROPERTY(isSend);
WCDB_PROPERTY(isShowTimer);
WCDB_PROPERTY(createTime);
WCDB_PROPERTY(content);
WCDB_PROPERTY(imgPath);
WCDB_PROPERTY(reserved);
WCDB_PROPERTY(lvBuffer);
WCDB_PROPERTY(transContent);
WCDB_PROPERTY(transBrandWording);
WCDB_PROPERTY(bizClientMsgId);
WCDB_PROPERTY(bizChatId);
WCDB_PROPERTY(bizChatUserId);
WCDB_PROPERTY(msgSeq);
WCDB_PROPERTY(flag);
WCDB_PROPERTY(solitaireFoldInfo);
WCDB_PROPERTY(historyId);

@end

//
// Created by qiuwenchen on 2020/8/30.
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

@interface FTS5RowidIndexItem : NSObject <WCTTableCoding>
@property (nonatomic, assign) UInt32 userNameId;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) SInt64 indexRowid;
WCDB_PROPERTY(userNameId);
WCDB_PROPERTY(msgLocalId);
WCDB_PROPERTY(indexRowid);
@end

@interface FTS3MsgContentItem : NSObject <WCTTableCoding>
@property (nonatomic, assign) UInt32 userNameId;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) UInt32 createTime;
@property (nonatomic, retain) NSString* msg;

WCDB_PROPERTY(userNameId);
WCDB_PROPERTY(msgLocalId);
WCDB_PROPERTY(createTime);
WCDB_PROPERTY(msg);
@end

@interface FTS5MsgContentItem : NSObject <WCTTableCoding>
@property (nonatomic, assign) UInt32 userNameId;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) UInt32 createTime;
@property (nonatomic, retain) NSString* msg;

WCDB_PROPERTY(userNameId);
WCDB_PROPERTY(msgLocalId);
WCDB_PROPERTY(createTime);
WCDB_PROPERTY(msg);
@end

@interface FTS5NewFavSearchItem : NSObject <WCTTableCoding>

@property (nonatomic, strong) NSString* searchTitle;
@property (nonatomic, strong) NSString* searchDesc;
@property (nonatomic, strong) NSString* searchSource;
@property (nonatomic, assign) UInt32 localId;
@property (nonatomic, assign) UInt32 updateTime;

WCDB_PROPERTY(searchTitle);
WCDB_PROPERTY(searchDesc);
WCDB_PROPERTY(searchSource);
WCDB_PROPERTY(localId);
WCDB_PROPERTY(updateTime);

@end

typedef enum : NSUInteger {
    FTSContactType_Normal = 0,
    FTSContactType_Chatroom,
    FTSContactType_Brand,
    FTSContactType_OpenIm,
} FTSContactType;

@interface FTS5ContactSearchItem : NSObject <WCTTableCoding>

@property (nonatomic, assign) UInt32 listType;
@property (nonatomic, assign) FTSContactType contactType;
@property (nonatomic, strong) NSString* userName;
@property (nonatomic, strong) NSString* mainSearchContent;
@property (nonatomic, strong) NSString* zone;
//利用fts的分词特性，支持在一串username组成的字符串中搜索某个username，实现搜索一个set的效果。
@property (nonatomic, strong) NSString* groupMembers;       //以;为分割符。
@property (nonatomic, strong) NSString* associateChatRooms; //以;为分割符。

WCDB_PROPERTY(listType);
WCDB_PROPERTY(contactType);
WCDB_PROPERTY(userName);
WCDB_PROPERTY(mainSearchContent);
WCDB_PROPERTY(zone);
WCDB_PROPERTY(groupMembers)
WCDB_PROPERTY(associateChatRooms)

@end

//
// Created by 陈秋文 on 2020/8/30.
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
#import "FTSData.h"
#import <WCDB/WCTDatabase+FTS.h>

@implementation FTS5RowidIndexItem

WCDB_IMPLEMENTATION(FTS5RowidIndexItem)
WCDB_SYNTHESIZE_COLUMN(userNameId, WCDB_STRINGIFY(userNameId))
WCDB_SYNTHESIZE_COLUMN(msgLocalId, WCDB_STRINGIFY(msgLocalId))
WCDB_SYNTHESIZE_COLUMN(indexRowid, WCDB_STRINGIFY(indexRowid))

WCDB_MULTI_PRIMARY("userNameId_msgLocalId", userNameId)
WCDB_MULTI_PRIMARY("userNameId_msgLocalId", msgLocalId)
WCDB_WITHOUT_ROWID

@end

@interface FTS3MsgContentItem ()
@property (nonatomic, assign) UInt32 reservedInt;
@property (nonatomic, retain) NSString *reservedText;

WCDB_PROPERTY(reservedInt);
WCDB_PROPERTY(reservedText);
@end

@implementation FTS3MsgContentItem
@synthesize userNameId = userNameId;
@synthesize msgLocalId = msgLocalId;
@synthesize createTime = createTime;
@synthesize msg = msg;

WCDB_IMPLEMENTATION(FTS3MsgContentItem)
WCDB_SYNTHESIZE_COLUMN(userNameId, WCDB_STRINGIFY(COL_FTS_MSG_USERNAME_ID))
WCDB_SYNTHESIZE_COLUMN(msgLocalId, WCDB_STRINGIFY(COL_FTS_MSG_MES_LOCAL_ID))
WCDB_SYNTHESIZE_COLUMN(createTime, WCDB_STRINGIFY(COL_FTS_MSG_CREATE_TIME))
WCDB_SYNTHESIZE_COLUMN(msg, WCDB_STRINGIFY(COL_FTS_MSG_MESSAGE))

WCDB_SYNTHESIZE(reservedInt)
WCDB_SYNTHESIZE(reservedText)

WCDB_VIRTUAL_TABLE_MODULE(WCTModuleFTS3)
//WCDB_VIRTUAL_TABLE_TOKENIZE(WCTTokenizerOneWord)
WCDB_NOTINDEXED(userNameId)
WCDB_NOTINDEXED(msgLocalId)
WCDB_NOTINDEXED(createTime)
WCDB_NOTINDEXED(reservedInt)
WCDB_NOTINDEXED(reservedText)

WCDB_MULTI_PRIMARY("userNameId_msgLocalId_createTime", userNameId)
WCDB_MULTI_PRIMARY("userNameId_msgLocalId_createTime", msgLocalId)
WCDB_MULTI_PRIMARY("userNameId_msgLocalId_createTime", createTime)

WCDB_DEFAULT(reservedInt, 0)
WCDB_DEFAULT(userNameId, 0)
WCDB_DEFAULT(createTime, 0)

- (id)init {
    if (self = [super init]) {
    }
    return self;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"%d %d %d %@", self.userNameId, self.msgLocalId, self.createTime, self.msg];
}

@end

@interface FTS5MsgContentItem ()
@property (nonatomic, assign) UInt32 reservedInt;
@property (nonatomic, retain) NSString *reservedText;

WCDB_PROPERTY(reservedInt);
WCDB_PROPERTY(reservedText);
@end

@implementation FTS5MsgContentItem
@synthesize userNameId = userNameId;
@synthesize msgLocalId = msgLocalId;
@synthesize createTime = createTime;
@synthesize msg = msg;

WCDB_IMPLEMENTATION(FTS5MsgContentItem)
WCDB_SYNTHESIZE_COLUMN(msg, WCDB_STRINGIFY(COL_FTS_MSG_MESSAGE))
WCDB_SYNTHESIZE_COLUMN(userNameId, WCDB_STRINGIFY(COL_FTS_MSG_USERNAME_ID))
WCDB_SYNTHESIZE_COLUMN(msgLocalId, WCDB_STRINGIFY(COL_FTS_MSG_MES_LOCAL_ID))
WCDB_SYNTHESIZE_COLUMN(createTime, WCDB_STRINGIFY(COL_FTS_MSG_CREATE_TIME))
WCDB_SYNTHESIZE(reservedInt)
WCDB_SYNTHESIZE(reservedText)

WCDB_VIRTUAL_TABLE_MODULE(WCTModuleFTS5)
//WCDB_VIRTUAL_TABLE_TOKENIZE(WCTTokenizerOneWord_FTS5)

WCDB_MULTI_PRIMARY("userNameId_msgLocalId_createTime", userNameId)
WCDB_MULTI_PRIMARY("userNameId_msgLocalId_createTime", msgLocalId)
WCDB_MULTI_PRIMARY("userNameId_msgLocalId_createTime", createTime)

WCDB_DEFAULT(reservedInt, 0)
WCDB_DEFAULT(userNameId, 0)
WCDB_DEFAULT(createTime, 0)

WCDB_NOTINDEXED(userNameId)
WCDB_NOTINDEXED(msgLocalId)
WCDB_NOTINDEXED(createTime)
WCDB_NOTINDEXED(reservedInt)
WCDB_NOTINDEXED(reservedText)

- (id)init {
    if (self = [super init]) {
    }
    return self;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"%d %d %d %@", self.userNameId, self.msgLocalId, self.createTime, self.msg];
}

@end

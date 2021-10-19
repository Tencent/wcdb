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

#import "FTSData.h"
#import <Foundation/Foundation.h>
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

WCDB_VIRTUAL_TABLE_MODULE(WCTModuleFTS4)
WCDB_VIRTUAL_TABLE_TOKENIZE(WCTTokenizerOneOrBinary)
WCDB_VIRTUAL_TABLE_EXTERNAL_CONTENT("")
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

- (id)init
{
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
WCDB_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS(WCTTokenizerOneOrBinary_FTS5, WCTTokenizerParameter_OneWord)
WCDB_VIRTUAL_TABLE_EXTERNAL_CONTENT("")

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

- (id)init
{
    if (self = [super init]) {
    }
    return self;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"%d %d %d %@", self.userNameId, self.msgLocalId, self.createTime, self.msg];
}

@end

@interface FTS5NewFavSearchItem ()
@property (nonatomic, assign) UInt32 FavItemReservedInt;
@property (nonatomic, retain) NSString *FavItemReservedText;

WCDB_PROPERTY(FavItemReservedInt);
WCDB_PROPERTY(FavItemReservedText);
@end

@implementation FTS5NewFavSearchItem

@synthesize localId = localId;
@synthesize updateTime = updateTime;
@synthesize searchTitle = searchTitle;
@synthesize searchDesc = searchDesc;
@synthesize searchSource = searchSource;

WCDB_IMPLEMENTATION(FTS5NewFavSearchItem)
//参与建索引的列放在前面，并且按照该列的内容长度从大到小排列，有利于减小索引占用空间。
//因为WCDB还是会强制将列按照属性名的字母序排序，列名前面放个A、B、C可以保证列的顺序。
WCDB_SYNTHESIZE_COLUMN(searchTitle, "AFavItemSearchTitle")
WCDB_SYNTHESIZE_COLUMN(searchDesc, "BFavItemSearchDesc")
WCDB_SYNTHESIZE_COLUMN(searchSource, "CFavItemSearchSource")
WCDB_SYNTHESIZE_COLUMN(localId, WCDB_STRINGIFY(COL_FTS_FAV_ITEM_LOCAL_ID))
WCDB_SYNTHESIZE_COLUMN(updateTime, WCDB_STRINGIFY(COL_FTS_FAV_ITEM_UPDATE_TIME))

WCDB_SYNTHESIZE(FavItemReservedInt)
WCDB_SYNTHESIZE(FavItemReservedText)

WCDB_VIRTUAL_TABLE_MODULE(WCTModuleFTS5)
WCDB_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS(WCTTokenizerOneOrBinary_FTS5, WCTTokenizerParameter_OneWord)

WCDB_DEFAULT(localId, 0)
WCDB_DEFAULT(updateTime, 0)
WCDB_DEFAULT(FavItemReservedInt, 0)

WCDB_NOTINDEXED(localId)
WCDB_NOTINDEXED(updateTime)
WCDB_NOTINDEXED(FavItemReservedInt)
WCDB_NOTINDEXED(FavItemReservedText)

- (id)init
{
    if (self = [super init]) {
        self.localId = 0;
        self.updateTime = 0;
        self.searchTitle = nil;
        self.searchDesc = nil;
        self.searchSource = nil;
    }

    return self;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"localId=%u, title=%@", (unsigned int) localId, searchTitle];
}

@end

@interface FTS5ContactSearchItem ()

@property (nonatomic, retain) NSString *reservedText1;
@property (nonatomic, retain) NSString *reservedText2;

WCDB_PROPERTY(reservedText1);
WCDB_PROPERTY(reservedText2);
@end

@implementation FTS5ContactSearchItem

WCDB_IMPLEMENTATION(FTS5ContactSearchItem)
//mainSearchContent的内容比较多，放在前面可以节省空间
WCDB_SYNTHESIZE_COLUMN(mainSearchContent, "AMainSearchContent")
WCDB_SYNTHESIZE_COLUMN(groupMembers, "BGroupMembers")
WCDB_SYNTHESIZE_COLUMN(userName, WCDB_STRINGIFY(userName))
WCDB_SYNTHESIZE_COLUMN(zone, WCDB_STRINGIFY(zone))
WCDB_SYNTHESIZE_COLUMN(reservedText1, "zzreservedText1")
WCDB_SYNTHESIZE_COLUMN(reservedText2, "zzreservedText2")
//以下内容不需要建索引，放到最后，可以减少存储空间
WCDB_SYNTHESIZE_COLUMN(associateChatRooms, "zzAssociateChatRooms")
WCDB_SYNTHESIZE_COLUMN(listType, "zzlistType")
WCDB_SYNTHESIZE_COLUMN(contactType, "zzContactType")

WCDB_VIRTUAL_TABLE_MODULE(WCTModuleFTS5)
WCDB_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS(WCTTokenizerOneOrBinary_FTS5, WCTTokenizerParameter_OneWord, WCTTokenizerParameter_NeedSymbol)

WCDB_DEFAULT(listType, 0)
WCDB_DEFAULT(contactType, 0)
WCDB_NOTINDEXED(listType)
WCDB_NOTINDEXED(contactType)
WCDB_NOTINDEXED(associateChatRooms)

@end

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

#import "FTSFactory.h"
#import "FTSData.h"
#import "NSObject+TestCase.h"
#import "Random+FTSData.h"
#import "TestCaseAssertion.h"
#import "WCTDatabase+TestCase.h"
#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@implementation FTSFactory {
    NSDate* m_startProduceTime;
    int m_batchInsertCount;
}

- (instancetype)initWithDirectory:(NSString*)directory
{
    if (self = [super initWithDirectory:directory]) {
        self.delegate = self;
        m_batchInsertCount = 100;
    }
    return self;
}

- (NSString*)indexTableNameOf:(int)tableId
{
    if (tableId == 0) {
        return @"indexTable";
    }
    return [NSString stringWithFormat:@"indexTable_%d", tableId];
}

- (Class)getIndexORMClass
{
    switch (self.dataType) {
    case FTSDataType_FTS3:
        return FTS3MsgContentItem.class;
    case FTSDataType_FTS5:
        return FTS5MsgContentItem.class;
    case FTSDataType_FTS5_RowidIndex:
        return FTS5MsgContentItem.class;
    case FTSDataType_Fav:
        return FTS5NewFavSearchItem.class;
    case FTSDataType_Contact:
        return FTS5ContactSearchItem.class;
    }
}

- (NSString*)assistTableName
{
    return @"metaTable";
}

- (void)produce:(NSString*)destination
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        [[NSFileManager defaultManager] setFileImmutable:NO ofItemsIfExistsAtPath:self.path];

        if ([[NSFileManager defaultManager] fileExistsAtPath:self.database.path] && [self.database tableExists:[self indexTableNameOf:0]]) {
            int freePageCount = [self.database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::freelistCount())].numberValue.intValue;
            if (freePageCount > 1000) {
                TestCaseAssertTrue([self.database execute:WCDB::StatementVacuum().vacuum()]);
            }
            [self tryMergeOrOptimize:self.database];
            TestCaseAssertTrue([self.database truncateCheckpoint]);
        }
        [[NSFileManager defaultManager] setFileImmutable:YES
                                   ofItemsIfExistsAtPath:self.path];
    });
    return [super produce:destination];
}

- (void)preparePrototype:(WCTDatabase*)prototype currentQuality:(double)quality
{
    if (quality == 0) {
        m_startProduceTime = [NSDate date];
    }
    int maxNumberOfObjects = (int) self.quality;
    int existingNumberOfObjects = (int) quality;
    int numberOfObjects = (int) MAX((self.quality * 0.1), 1);
    if (maxNumberOfObjects - existingNumberOfObjects < numberOfObjects) {
        numberOfObjects = maxNumberOfObjects - existingNumberOfObjects;
    }

    if (existingNumberOfObjects == 0) {
        for (int i = 0; i < self.tableCount; i++) {
            TestCaseAssertTrue([prototype createVirtualTable:[self indexTableNameOf:i] withClass:[self getIndexORMClass]]);
        }
        if (self.dataType == FTSDataType_FTS5_RowidIndex) {
            TestCaseAssertTrue([prototype createTable:self.assistTableName withClass:FTS5RowidIndexItem.class]);
        }
    }

    for (int needInsert = numberOfObjects; needInsert > 0; needInsert -= m_batchInsertCount) {
        switch (self.dataType) {
        case FTSDataType_FTS3: {
            NSArray* objects = [[Random shared] randomFTS3Items:MIN(needInsert, m_batchInsertCount)];
            for (int i = 0; i < self.tableCount; i++) {
                TestCaseAssertTrue([prototype insertObjects:[objects subarrayWithRange:NSMakeRange(m_batchInsertCount * i / self.tableCount, m_batchInsertCount * (i + 1) / self.tableCount - m_batchInsertCount * i / self.tableCount)] intoTable:[self indexTableNameOf:i]]);
            }

        } break;
        case FTSDataType_FTS5: {
            NSArray* objects = [[Random shared] randomFTS5Items:MIN(needInsert, m_batchInsertCount)];
            for (int i = 0; i < self.tableCount; i++) {
                TestCaseAssertTrue([prototype insertObjects:[objects subarrayWithRange:NSMakeRange(m_batchInsertCount * i / self.tableCount, m_batchInsertCount * (i + 1) / self.tableCount - m_batchInsertCount * i / self.tableCount)] intoTable:[self indexTableNameOf:i]]);
            }
        } break;
        case FTSDataType_FTS5_RowidIndex: {
            [prototype runTransaction:^BOOL(WCTHandle* handle) {
                NSArray<FTS5MsgContentItem*>* objects = [[Random shared] randomFTS5Items:MIN(needInsert, m_batchInsertCount)];
                for (int i = 0; i < self.tableCount; i++) {
                    for (FTS5MsgContentItem* item in [objects subarrayWithRange:NSMakeRange(m_batchInsertCount * i / self.tableCount, m_batchInsertCount * (i + 1) / self.tableCount - m_batchInsertCount * i / self.tableCount)]) {
                        TestCaseAssertTrue([handle insertObject:item intoTable:[self indexTableNameOf:i]]);
                        FTS5RowidIndexItem* rowidIndex = [[FTS5RowidIndexItem alloc] init];
                        rowidIndex.msgLocalId = item.msgLocalId;
                        rowidIndex.userNameId = item.userNameId;
                        rowidIndex.indexRowid = handle.getLastInsertedRowID;
                        TestCaseAssertTrue([handle insertObject:rowidIndex intoTable:self.assistTableName]);
                    }
                }

                return YES;
            }];
        } break;
        case FTSDataType_Fav: {
            NSArray* objects = [[Random shared] randomFavFTSItem:MIN(needInsert, m_batchInsertCount)];
            for (int i = 0; i < self.tableCount; i++) {
                TestCaseAssertTrue([prototype insertObjects:[objects subarrayWithRange:NSMakeRange(m_batchInsertCount * i / self.tableCount, m_batchInsertCount * (i + 1) / self.tableCount - m_batchInsertCount * i / self.tableCount)] intoTable:[self indexTableNameOf:i]]);
            }
        } break;
        case FTSDataType_Contact: {
            NSArray* objects = [[Random shared] randomContactFTSItem:MIN(needInsert, m_batchInsertCount)];
            for (int i = 0; i < self.tableCount; i++) {
                TestCaseAssertTrue([prototype insertObjects:[objects subarrayWithRange:NSMakeRange(m_batchInsertCount * i / self.tableCount, m_batchInsertCount * (i + 1) / self.tableCount - m_batchInsertCount * i / self.tableCount)] intoTable:[self indexTableNameOf:i]]);
            }
        } break;
        }
    }

    if (existingNumberOfObjects + numberOfObjects < self.quality * (1.0 - self.tolerance)) {
        return;
    }

    TestCaseLog(@"DataType %d prepare total time: %.2f", self.dataType, [[NSDate date] timeIntervalSinceDate:m_startProduceTime]);

    [self tryMergeOrOptimize:prototype];

    int freePageCount = [prototype getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::freelistCount())].numberValue.intValue;
    if (freePageCount > 1000) {
        TestCaseAssertTrue([prototype execute:WCDB::StatementVacuum().vacuum()]);
        TestCaseAssertTrue([prototype truncateCheckpoint]);
    }
}

- (void)tryMergeOrOptimize:(WCTDatabase*)prototype
{
    if (self.needOptimize) {
        NSDate* start = [NSDate date];
        for (int i = 0; i < self.tableCount; i++) {
            WCDB::StatementInsert optimize;
            optimize.insertIntoTable([self indexTableNameOf:i]).column(WCDB::Column([self indexTableNameOf:i])).value(@"optimize");
            TestCaseAssertTrue([prototype execute:optimize]);
        }
        TestCaseLog(@"DataType %d optimize time: %.2f", self.dataType, [[NSDate date] timeIntervalSinceDate:start]);
        return;
    } else if (self.autoMergeCount > 0) {
        NSDate* start = [NSDate date];
        __block int mergeCount = 0;
        for (int i = 0; i < self.tableCount; i++) {
            TestCaseAssertTrue([prototype execute:WCDB::StatementInsert().insertIntoTable([self indexTableNameOf:i]).columns({ WCDB::Column([self indexTableNameOf:i]), WCDB::Column(@"rank") }).values({ @"usermerge", self.autoMergeCount })]);
            __block int preTotalChange = 0;
            TestCaseAssertTrue([prototype runTransaction:^BOOL(WCTHandle* handle) {
                do {
                    preTotalChange = [handle getTotalChanges];
                    TestCaseAssertTrue([handle execute:WCDB::StatementInsert().insertIntoTable([self indexTableNameOf:i]).columns({ WCDB::Column([self indexTableNameOf:i]), WCDB::Column(@"rank") }).values({ @"merge", 16 })]);
                    mergeCount++;
                } while ([handle getTotalChanges] - preTotalChange > 1);
                return true;
            }]);
        }
        TestCaseLog(@"DataType %d merge time: %.2f, merge count %d", self.dataType, [[NSDate date] timeIntervalSinceDate:start], mergeCount);
    }
}

- (double)qualityOfPrototype:(WCTDatabase*)prototype
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:prototype.path]) {
        NSDate* start = [NSDate date];
        if (![prototype tableExists:[self indexTableNameOf:0]]) {
            return 0;
        }
        WCTValue* count;
        int totalCount = 0;
        switch (self.dataType) {
        case FTSDataType_FTS3:
            for (int i = 0; i < self.tableCount; i++) {
                count = [prototype getValueOnResultColumn:FTS3MsgContentItem.allProperties.count() fromTable:[self indexTableNameOf:i]];
                TestCaseAssertNotNil(count);
                totalCount += count.numberValue.intValue;
            }

            break;
        case FTSDataType_FTS5:
            for (int i = 0; i < self.tableCount; i++) {
                count = [prototype getValueOnResultColumn:FTS5MsgContentItem.allProperties.count() fromTable:[self indexTableNameOf:i]];
                TestCaseAssertNotNil(count);
                totalCount += count.numberValue.intValue;
            }
            break;
        case FTSDataType_FTS5_RowidIndex:
            count = [prototype getValueFromStatement:WCDB::StatementSelect().select(FTS5RowidIndexItem.allProperties.count()).from(self.assistTableName)];
            TestCaseAssertNotNil(count);
            totalCount = count.numberValue.intValue;
            break;
        case FTSDataType_Fav: {
            for (int i = 0; i < self.tableCount; i++) {
                count = [prototype getValueOnResultColumn:FTS5NewFavSearchItem.allProperties.count() fromTable:[self indexTableNameOf:i]];
                TestCaseAssertNotNil(count);
                totalCount += count.numberValue.intValue;
            }
        } break;
        case FTSDataType_Contact: {
            for (int i = 0; i < self.tableCount; i++) {
                count = [prototype getValueOnResultColumn:FTS5ContactSearchItem.allProperties.count() fromTable:[self indexTableNameOf:i]];
                TestCaseAssertNotNil(count);
                totalCount += count.numberValue.intValue;
            }
        } break;
        }
        TestCaseLog(@"get quality cost time %.2f", [[NSDate date] timeIntervalSinceDate:start]);
        return totalCount;
    }
    return 0;
}

- (NSString*)categoryOfPrototype
{
    NSString* category;
    switch (self.dataType) {
    case FTSDataType_FTS3:
        category = @"FTS3";
        break;
    case FTSDataType_FTS5:
        category = @"FTS5";
        break;
    case FTSDataType_FTS5_RowidIndex:
        category = @"FTS5_Rowid";
        break;
    case FTSDataType_Fav:
        category = @"FTS5_Fav";
        break;
    case FTSDataType_Contact:
        category = @"FTS5_Contact";
        break;
    }
    if (self.tokenizerName.length > 0) {
        NSRange range = [self.tokenizerName rangeOfString:WCTTokenizerOneOrBinary];
        NSString* tokenizerParameter = [[self.tokenizerName substringFromIndex:NSMaxRange(range)] stringByReplacingOccurrencesOfString:@" " withString:@"-"];
        category = [category stringByAppendingString:tokenizerParameter];
    }
    if (self.tableCount > 0) {
        category = [category stringByAppendingFormat:@"_Table%d", self.tableCount];
    }
    if (self.needOptimize) {
        return [category stringByAppendingString:@"_Opt"];
    } else if (self.autoMergeCount > 0) {
        return [category stringByAppendingFormat:@"_%d", self.autoMergeCount];
    } else {
        return category;
    }
}

@end

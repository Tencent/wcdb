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

#import "FTSTests.h"
#import <Foundation/Foundation.h>

@implementation FTSTests {
    FTSFactory* m_factory;
    FTSDataType m_dataType;
    int m_tableCount;
    NSString* m_tokenizer;
    int m_quality;
    int m_optimizeLevel;
    int m_querylevel;
    int m_queryTimes;
    int m_queryType;
    bool m_needMultiThread;
    int m_resultCount;
}

- (FTSFactory*)factory
{
    @synchronized(self) {
        if (m_factory == nil) {
            m_factory = [[FTSFactory alloc] initWithDirectory:self.class.cacheRoot];
        }
        return m_factory;
    }
}

- (NSString*)query
{
    if (m_dataType == FTSDataType_FTS5) {
        switch (m_querylevel) {
        case 1:
            return @"狱咋射石";
        case 2:
            return @"中等规模随机命中";
        case 3:
            return @"大规模批量测试内容";
        }
    } else {
        switch (m_querylevel) {
        case 1:
            return @"\"狱咋\" \"射石\"";
        case 2:
            return @"\"中等\" \"规模\" \"随机\" \"命中\"";
        case 3:
            return @"\"大\" \"规模\" \"批量\" \"测试\" \"内容\"";
        }
    }
    assert(0);
    return 0;
}

- (Class)getIndexORMClass
{
    switch (m_dataType) {
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

- (NSString*)tokenizerName
{
    if (m_dataType == FTSDataType_FTS3) {
        return WCTTokenizerOneOrBinary;
    } else {
        return WCTTokenizerOneOrBinary_FTS5;
    }
}

- (int)getTokenizerIdWithNeedBinary:(BOOL)needBinary needPinyin:(BOOL)needPinyin needSymbol:(BOOL)needSymbol
{
    if (needBinary) {
        if (needPinyin) {
            return 0;
        } else {
            if (needSymbol) {
                return 1;
            } else {
                return 2;
            }
        }
    } else {
        if (needPinyin) {
            return 3;
        } else {
            if (needSymbol) {
                return 4;
            } else {
                return 5;
            }
        }
    }
}

- (NSString*)getTokenizerWithId:(int)tkId
{
    NSString* name = [self tokenizerName];
    NSArray* tokenizers = @[
        [NSString stringWithUTF8String:WCTFTSTokenizerUtil::tokenize(name, WCTTokenizerParameter_PinYin, nil).data()],
        [NSString stringWithUTF8String:WCTFTSTokenizerUtil::tokenize(name, WCTTokenizerParameter_NeedSymbol, nil).data()],
        [NSString stringWithUTF8String:WCTFTSTokenizerUtil::tokenize(name, nil).data()],
        [NSString stringWithUTF8String:WCTFTSTokenizerUtil::tokenize(name, WCTTokenizerParameter_OneWord, WCTTokenizerParameter_PinYin, nil).data()],
        [NSString stringWithUTF8String:WCTFTSTokenizerUtil::tokenize(name, WCTTokenizerParameter_OneWord, WCTTokenizerParameter_NeedSymbol, nil).data()],
        [NSString stringWithUTF8String:WCTFTSTokenizerUtil::tokenize(name, WCTTokenizerParameter_OneWord, nil).data()]
    ];
    return tokenizers[tkId];
}

- (void)setUp
{
    [super setUp];
    m_queryTimes = 100;
}

- (NSString*)indexTableNameOfId:(int)tableId
{
    return [self.factory indexTableNameOf:tableId];
}

- (void)setUpDatabase
{
    [[Random shared] reset];
    self.factory.tokenizerName = m_tokenizer;
    self.factory.quality = m_quality;
    self.factory.tableCount = m_tableCount;
    self.factory.tolerance = (double) 100 / self.factory.quality;
    self.factory.needOptimize = m_optimizeLevel == 0;
    self.factory.autoMergeCount = MAX(0, m_optimizeLevel);
    self.factory.dataType = m_dataType;

    WCDB::StatementCreateVirtualTable& statement = const_cast<WCTBinding&>([[self getIndexORMClass] objectRelationalMapping]).statementVirtualTable;
    WCDB::Syntax::CreateVirtualTableSTMT& statementSyntax = statement.syntax();
    auto iter = statementSyntax.arguments.begin();
    while (iter != statementSyntax.arguments.end()) {
        if (iter->find(WCDB::Syntax::CreateVirtualTableSTMT::tokenizerPreFix()) == 0) {
            iter = statementSyntax.arguments.erase(iter);
        } else {
            ++iter;
        }
    }
    statementSyntax.arguments.push_back(WCDB::StringView(m_tokenizer.UTF8String));

    [self.factory.database addTokenizer:[self tokenizerName]];
    [self.database addTokenizer:[self tokenizerName]];
    [WCTDatabase configPinYinDict:@{
        @"单" : @[ @"shan", @"dan", @"chan" ],
        @"于" : @[ @"yu" ],
        @"骑" : @[ @"qi" ],
        @"模" : @[ @"mo", @"mu" ],
        @"具" : @[ @"ju" ],
        @"车" : @[ @"che" ],
    }];

    [self.factory produce:self.path];
    self.assistTableName = self.factory.assistTableName;

    TestCaseAssertOptionalEqual([self.database getNumberOfWalFrames], 0);
    TestCaseAssertFalse([self.database isOpened]);
}

- (void)measureTest:(void (^)(void))block
{
    NSMutableString* timeString = [[NSMutableString alloc] init];
    double totalTime = 0;
    [self setUpDatabase];
    for (int i = 0; i < 10; i++) {
        NSDate* start = [NSDate date];
        for (int j = 0; j < m_queryTimes; j++) {
            m_resultCount = 0;
            block();
        }
        double costTime = [[NSDate date] timeIntervalSinceDate:start];
        totalTime += costTime;
        [timeString appendFormat:@"%.6f, ", costTime];
    }
    TestCaseLog(@"Query type %d result count %d", m_queryType, m_resultCount);
    TestCaseLog(@"Time cost for dataType %d, %@, quality %d, optimizeLevel %d, querylevel %d, queryTimes %d, queryType %d :", m_dataType, m_tokenizer, m_quality, m_optimizeLevel, m_querylevel, m_queryTimes, m_queryType);
    TestCaseLog(@"Avergae time %0.6f, all time %@", totalTime / 10, timeString);

    [self tearDownDatabase];
}

- (void)queryInAllTable:(void (^)(NSString* tableName))block
{
    if (self->m_needMultiThread) {
        dispatch_group_t group = dispatch_group_create();
        dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
        for (int i = 0; i < self->m_tableCount; i++) {
            dispatch_group_async(group, queue, ^() {
                block([self indexTableNameOfId:i]);
            });
        }
        dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
    } else {
        for (int i = 0; i < self->m_tableCount; i++) {
            block([self indexTableNameOfId:i]);
        }
    }
}

- (void)tearDownDatabase
{
    [self.database removeFiles];
    self.path = nil;
    m_factory = nil;
}

- (NSArray<NSNumber*>*)genTestCaseConfigDataType:(FTSDataType)dataType
                                      tableCount:(int)tableCount
                                     tokenizerId:(int)tokenizerId
                                         quality:(int)quality
                                   optimizeLevel:(int)optimizeLevel
                                      querylevel:(int)querylevel
                                      queryTimes:(int)queryTimes
                                       queryType:(int)queryType
                                 needMultiThread:(bool)needMultiThread
{
    return @[ @(dataType), @(tableCount), @(tokenizerId), @(quality), @(optimizeLevel), @(querylevel), @(queryTimes), @(queryType), @(needMultiThread) ];
}

- (void)applyConfig:(NSArray<NSNumber*>*)configs
{
    m_dataType = (FTSDataType) configs[0].intValue;
    m_tableCount = configs[1].intValue;
    m_tokenizer = [self getTokenizerWithId:configs[2].intValue];
    m_quality = configs[3].intValue;
    m_optimizeLevel = configs[4].intValue;
    m_querylevel = configs[5].intValue;
    m_queryTimes = configs[6].intValue;
    m_queryType = configs[7].intValue;
    m_needMultiThread = configs[8].boolValue;
}

#pragma mark - Tests

- (void)test_query
{
    NSMutableArray<NSArray<NSNumber*>*>* testCases = [[NSMutableArray alloc] init];
    NSArray<NSNumber*>* testCase;

    int tkId = [self getTokenizerIdWithNeedBinary:NO needPinyin:NO needSymbol:NO];
    testCase = [self genTestCaseConfigDataType:FTSDataType_Contact tableCount:1 tokenizerId:tkId quality:1000000 optimizeLevel:2 querylevel:2 queryTimes:10 queryType:6 needMultiThread:NO];
    [testCases addObject:testCase];

    for (NSArray<NSNumber*>* config in testCases) {
        [self applyConfig:config];
        [self measureTest:^{
            switch (self->m_queryType) {
            case 0: {
                [self queryInAllTable:^(NSString* tableName) {
                    WCTValue* count = [self.database getValueOnResultColumn:FTS3MsgContentItem.allProperties.count() fromTable:tableName where:FTS3MsgContentItem.msg.match([self query])];
                    self->m_resultCount += count.numberValue.intValue;
                }];
            } break;
            case 1: {
                WCTProperties vecProperty;
                vecProperty.push_back(FTS3MsgContentItem.userNameId);
                vecProperty.push_back(FTS3MsgContentItem.msgLocalId);
                vecProperty.push_back(FTS3MsgContentItem.createTime);
                [self queryInAllTable:^(NSString* tableName) {
                    NSArray* result = [self.database getObjectsOnResultColumns:vecProperty
                                                                     fromTable:tableName
                                                                         where:FTS3MsgContentItem.msg.match([self query])
                                                                        orders:FTS5MsgContentItem.createTime];
                    self->m_resultCount += result.count;
                }];
            } break;
            case 2: {
                WCTProperties vecProperty;
                vecProperty.push_back(FTS3MsgContentItem.userNameId);
                vecProperty.push_back(FTS3MsgContentItem.msgLocalId);
                vecProperty.push_back(FTS3MsgContentItem.createTime);
                WCDB::Expressions expectedUserNameIds;
                expectedUserNameIds.push_back(@1);
                expectedUserNameIds.push_back(@2);
                expectedUserNameIds.push_back(@3);
                [self queryInAllTable:^(NSString* tableName) {
                    NSArray* result = [self.database getObjectsOnResultColumns:vecProperty
                                                                     fromTable:tableName
                                                                         where:FTS3MsgContentItem.userNameId.in(expectedUserNameIds) && FTS3MsgContentItem.msgLocalId < 1000000 && FTS3MsgContentItem.msg.match([self query])
                                                                        orders:{ FTS3MsgContentItem.createTime.asOrder(WCTOrderedDescending) }];
                    self->m_resultCount += result.count;
                }];
            } break;
            case 3: {
                [self queryInAllTable:^(NSString* tableName) {
                    WCTValue* count = [self.database getValueOnResultColumn:FTS5NewFavSearchItem.allProperties.count()
                                                                  fromTable:tableName
                                                                      where:FTS5NewFavSearchItem.searchTitle.match([self query]) || FTS5NewFavSearchItem.searchDesc.match([self query])
                                                                            || FTS5NewFavSearchItem.searchSource.match([self query])];
                    self->m_resultCount += count.numberValue.unsignedIntValue;
                }];
            } break;
            case 4: {
                [self queryInAllTable:^(NSString* tableName) {
                    WCTValue* count = [self.database getValueOnResultColumn:FTS5NewFavSearchItem.allProperties.count()
                                                                  fromTable:tableName
                                                                      where:FTS5NewFavSearchItem.searchTitle.match([self query])];
                    self->m_resultCount += count.numberValue.unsignedIntValue;
                    count = [self.database getValueOnResultColumn:FTS5NewFavSearchItem.allProperties.count()
                                                        fromTable:tableName
                                                            where:FTS5NewFavSearchItem.searchDesc.match([self query])];
                    self->m_resultCount += count.numberValue.unsignedIntValue;
                    count = [self.database getValueOnResultColumn:FTS5NewFavSearchItem.allProperties.count()
                                                        fromTable:tableName
                                                            where:FTS5NewFavSearchItem.searchSource.match([self query])];
                    self->m_resultCount += count.numberValue.unsignedIntValue;
                }];
            } break;
            case 5: {
                [self queryInAllTable:^(NSString* tableName) {
                    const WCDB::ResultColumn highlightColumn = WCDB::ResultColumn(WCDB::Expression::function("highlight").invoke().arguments({ WCDB::Column(tableName), 1, "<b>", "</b>" }));
                    WCDB::ResultColumns resultColumns;
                    resultColumns.push_back(FTS5ContactSearchItem.listType);
                    resultColumns.push_back(FTS5ContactSearchItem.contactType);
                    resultColumns.push_back(FTS5ContactSearchItem.userName);
                    resultColumns.push_back(FTS5ContactSearchItem.associateChatRooms);
                    //                    resultColumns.push_back(highlightColumn);
                    NSArray* result = [self.database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(FTS5ContactSearchItem.mainSearchContent.match([self query]))];
                    self->m_resultCount += result.count;
                }];
            } break;
            case 6: {
                [self queryInAllTable:^(NSString* tableName) {
                    const WCDB::ResultColumn highlightColumn = WCDB::ResultColumn(WCDB::Expression::function("highlight").invoke().arguments({ WCDB::Column(tableName), 1, "<b>", "</b>" }));
                    WCDB::ResultColumns resultColumns;
                    resultColumns.push_back(FTS5ContactSearchItem.listType);
                    resultColumns.push_back(FTS5ContactSearchItem.contactType);
                    resultColumns.push_back(FTS5ContactSearchItem.userName);
                    //                        resultColumns.push_back(highlightColumn);
                    WCTColumnsXRows* result = [self.database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(FTS5ContactSearchItem.mainSearchContent.match([self query]))];
                    self->m_resultCount += result.count;
                    WCTHandle* handle = [self.database getHandle];
                    TestCaseAssertTrue([handle prepare:WCDB::StatementSelect().select({ FTS5ContactSearchItem.userName, FTS5ContactSearchItem.groupMembers }).from(tableName).where(FTS5ContactSearchItem.groupMembers.match(WCDB::BindParameter(1)))]);
                    NSMutableSet* matchGroups = [[NSMutableSet alloc] init];
                    int normalContactCount = 0;
                    for (WCTOneRow* oneRow in result) {
                        if (oneRow[1].numberValue.intValue != FTSContactType_Normal) {
                            continue;
                        }
                        normalContactCount++;
                        TestCaseAssertTrue(oneRow[2].stringValue.length > 0);
                        [handle reset];
                        [handle bindString:oneRow[2].stringValue toIndex:1];
                        NSString* matchContent = [@";" stringByAppendingString:oneRow[2].stringValue];
                        while ([handle step] && ![handle done]) {
                            WCTOneRow* oneGroup = [handle extractRow];
                            if ([oneGroup[1].stringValue hasPrefix:oneRow[2].stringValue] || [oneGroup[1].stringValue containsString:matchContent]) {
                                [matchGroups addObject:oneGroup[0].stringValue];
                            }
                        }
                    }
                    NSLog(@"normal count %d", normalContactCount);
                    [handle invalidate];
                }];
            }
            default:
                break;
            }
        }];
    }
}

- (void)test_pinyin_quary
{
    NSMutableArray<NSArray<NSNumber*>*>* testCases = [[NSMutableArray alloc] init];
    NSArray<NSNumber*>* testCase;

    int tkId = [self getTokenizerIdWithNeedBinary:NO needPinyin:YES needSymbol:NO];
    testCase = [self genTestCaseConfigDataType:FTSDataType_FTS5 tableCount:4 tokenizerId:tkId quality:10000 optimizeLevel:2 querylevel:3 queryTimes:10 queryType:0 needMultiThread:NO];
    [testCases addObject:testCase];

    NSArray* querys = @[
        @"\"shan yu qi mu ju dan che\"",
        @"\"chan yu qi mo ju shan che\"",
        @"\"dan yu qi mo ju chan che\"",
        @"\"dan yu qi mu ju ch\"*",
        @"\"dan yu qi mo ju d\"*",
        @"\"s y q m j d c\"",
        @"\"c y q m j s c\"",
        @"\"c y q m j\"",
    ];

    for (NSArray<NSNumber*>* config in testCases) {
        [self applyConfig:config];
        [self measureTest:^{
            switch (self->m_queryType) {
            case 0: {
                int preresultCount = 0;
                for (NSString* query in querys) {
                    self->m_resultCount = 0;
                    [self queryInAllTable:^(NSString* tableName) {
                        NSArray* allObj = [self.database getObjectsOfClass:FTS3MsgContentItem.class fromTable:tableName where:FTS3MsgContentItem.msg.match(query)];
                        self->m_resultCount += allObj.count;
                    }];
                    if (preresultCount == 0) {
                        preresultCount = self->m_resultCount;
                    } else {
                        TestCaseAssertTrue(preresultCount == self->m_resultCount);
                    }
                }
            } break;
            }
        }];
    }
}

- (void)test_fts5_index_view
{
    int tkId = [self getTokenizerIdWithNeedBinary:NO needPinyin:NO needSymbol:NO];
    [self applyConfig:[self genTestCaseConfigDataType:FTSDataType_FTS5_RowidIndex tableCount:0 tokenizerId:tkId quality:1000000 optimizeLevel:0 querylevel:1 queryTimes:100 queryType:0 needMultiThread:NO]];
    [self setUpDatabase];

    [self.database execute:WCDB::StatementCreateVirtualTable().createVirtualTable(@"indexView").usingModule(@"fts5vocab").argument([self indexTableNameOfId:0]).argument(@"instance")];
    WCTColumnsXRows* indexInfo = [self.database getRowsFromStatement:WCDB::StatementSelect().select(WCDB::ResultColumn().all()).from(@"indexView")];
    for (NSArray<WCTValue*>* array in indexInfo) {
        NSLog(@"%@, %d, %@, %d", array[0].stringValue, array[1].numberValue.intValue, array[2].stringValue, array[3].numberValue.intValue);
    }

    [self tearDownDatabase];
}

- (void)test_delete_rowid
{
    int tkId = [self getTokenizerIdWithNeedBinary:NO needPinyin:NO needSymbol:NO];
    [self applyConfig:[self genTestCaseConfigDataType:FTSDataType_FTS5_RowidIndex tableCount:0 tokenizerId:tkId quality:1000000 optimizeLevel:0 querylevel:1 queryTimes:100 queryType:0 needMultiThread:NO]];

    [self measureTest:^() {
        [self.database runTransaction:^BOOL(WCTHandle* _Nonnull) {
            NSArray<FTS5RowidIndexItem*>* items = [self.database getObjectsOfClass:FTS5RowidIndexItem.class fromTable:self.assistTableName where:FTS5RowidIndexItem.userNameId == 1 orders:{ FTS5RowidIndexItem.indexRowid.asOrder(WCTOrderedDescending) } limit:100];
            NSLog(@"delete count %lu", (unsigned long) items.count);
            for (FTS5RowidIndexItem* item in items) {
                TestCaseAssertTrue([self.database deleteFromTable:self.assistTableName where:FTS5RowidIndexItem.userNameId == item.userNameId && FTS5RowidIndexItem.msgLocalId == item.msgLocalId && FTS5RowidIndexItem.indexRowid == item.indexRowid]);
                TestCaseAssertTrue([self.database deleteFromTable:[self indexTableNameOfId:0] where:WCDB::Column::rowid() == item.indexRowid]);
            }
            return YES;
        }];
    }];
}

@end

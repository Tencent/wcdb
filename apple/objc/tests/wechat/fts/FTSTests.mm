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
#import "FTSTests.h"

@implementation FTSTests {
    FTSFactory* m_factory;
    FTSDataType m_dataType;
    int m_tableCount;
    bool m_needBinary;
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
    if(m_dataType == FTSDataType_FTS3){
        switch (m_querylevel) {
            case 1:
                return @"狱咋射石";
            case 2:
                return @"中等规模随机命中";
            case 3:
                return @"我们批量测试内容";
        }
    }else{
        switch (m_querylevel) {
            case 1:
                return @"\"狱咋\" \"射石\"";
            case 2:
                return @"\"中等\" \"规模\" \"随机\" \"命中\"";
            case 3:
                return @"\"我们\" \"批量\" \"测试\" \"内容\"";
        }
    }
    assert(0);
    return 0;
}

-(Class)getIndexORMClass
{
    switch (m_dataType) {
        case FTSDataType_FTS3:
            return FTS3MsgContentItem.class;
        case FTSDataType_FTS5:
            return FTS5MsgContentItem.class;
        case FTSDataType_FTS5_RowidIndex:
            return FTS5MsgContentItem.class;
    }
}

-(NSString*)tokenizerName
{
    if(m_dataType == FTSDataType_FTS3){
        if(m_needBinary){
            return WCTTokenizerOneOrBinary;
        }else{
            return WCTTokenizerOneWord;
        }
    }else{
        if(m_needBinary){
            return WCTTokenizerOneOrBinary_FTS5;
        }else{
            return WCTTokenizerOneWord_FTS5;
        }
    }
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
    self.factory.needBinary = m_needBinary;
    self.factory.quality = m_quality;
    self.factory.tableCount = m_tableCount;
    self.factory.tolerance = (double)100 / self.factory.quality;
    self.factory.needOptimize = m_optimizeLevel == 0;
    self.factory.autoMergeCount = MAX(0, m_optimizeLevel);
    self.factory.dataType = m_dataType;
    
    WCDB::StatementCreateVirtualTable& statement = const_cast<WCTBinding &>([[self getIndexORMClass] objectRelationalMapping]).statementVirtualTable;
    WCDB::Syntax::CreateVirtualTableSTMT& statementSyntax = statement.syntax();
    statementSyntax.arguments.clear();
    statementSyntax.arguments.push_back(WCDB::StatementCreateVirtualTable::tokenize([self tokenizerName]));
    
    [self.factory.database addTokenizer:[self tokenizerName]];
    [self.database addTokenizer:[self tokenizerName]];
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
    for(int i = 0; i < 10; i++){
        NSDate* start = [NSDate date];
        for(int j = 0; j < m_queryTimes; j++){
            m_resultCount = 0;
            block();
        }
        double costTime = [[NSDate date] timeIntervalSinceDate:start];
        totalTime += costTime;
        [timeString appendFormat:@"%.6f, ", costTime];
    }
    TestCaseLog(@"Query type %d result count %d", m_queryType, m_resultCount);
    TestCaseLog(@"Time cost for dataType %d, needBinary %d, quality %d, optimizeLevel %d, querylevel %d, queryTimes %d, queryType %d :", m_dataType, m_needBinary, m_quality, m_optimizeLevel, m_querylevel, m_queryTimes, m_queryType);
    TestCaseLog(@"Avergae time %0.6f, all time %@", totalTime / 10, timeString);
    
    [self tearDownDatabase];
}

- (void)tearDownDatabase
{
    [self.database removeFiles];
    self.path = nil;
    m_factory = nil;
}

- (NSArray<NSNumber*>*)genTestCaseConfigDataType:(FTSDataType)dataType
                                      tableCount:(int)tableCount
                                      needBinary:(BOOL)needBinary
                                         quality:(int)quality
                                   optimizeLevel:(int)optimizeLevel
                                      querylevel:(int)querylevel
                                      queryTimes:(int)queryTimes
                                       queryType:(int)queryType
                                 needMultiThread:(bool)needMultiThread
{
    return @[@(dataType), @(tableCount), @(needBinary), @(quality), @(optimizeLevel), @(querylevel), @(queryTimes), @(queryType), @(needMultiThread)];
}

- (void)applyConfig:(NSArray<NSNumber*>*)configs
{
    m_dataType = (FTSDataType)configs[0].intValue;
    m_tableCount = configs[1].intValue;
    m_needBinary = (BOOL)configs[2].boolValue;
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
    
    testCase = [self genTestCaseConfigDataType:FTSDataType_FTS5_RowidIndex tableCount:4 needBinary:NO quality:1000000 optimizeLevel:2 querylevel:2 queryTimes:10 queryType:0 needMultiThread:NO];
    [testCases addObject:testCase];
    
    testCase = [self genTestCaseConfigDataType:FTSDataType_FTS5_RowidIndex tableCount:4 needBinary:NO quality:1000000 optimizeLevel:2 querylevel:3 queryTimes:10 queryType:0 needMultiThread:NO];
    [testCases addObject:testCase];
    
    testCase = [self genTestCaseConfigDataType:FTSDataType_FTS3 tableCount:10 needBinary:YES quality:1000000 optimizeLevel:-1 querylevel:2 queryTimes:10 queryType:0 needMultiThread:NO];
    [testCases addObject:testCase];
    
    testCase = [self genTestCaseConfigDataType:FTSDataType_FTS3 tableCount:10 needBinary:YES quality:1000000 optimizeLevel:-1 querylevel:3 queryTimes:10 queryType:0 needMultiThread:NO];
    [testCases addObject:testCase];
    
    
    for(NSArray<NSNumber*>* config in testCases){
        [self applyConfig:config];
        [self measureTest:^{
            switch (self->m_queryType) {
                case 0:{
                    WCTValue* count;
                    for(int i = 0; i<self->m_tableCount; i++){
                        count = [self.database getValueOnResultColumn:FTS3MsgContentItem.allProperties.count() fromTable:[self indexTableNameOfId:i]  where:FTS3MsgContentItem.msg.match([self query])];
                        self->m_resultCount += count.numberValue.intValue;
                    }
                }break;
                case 1:{
                    WCTProperties vecProperty;
                    vecProperty.push_back(FTS3MsgContentItem.userNameId);
                    vecProperty.push_back(FTS3MsgContentItem.msgLocalId);
                    vecProperty.push_back(FTS3MsgContentItem.createTime);
                    vecProperty.push_back(FTS3MsgContentItem.msg);
                    NSArray* result;
                    for(int i = 0; i<self->m_tableCount; i++){
                        result = [self.database getObjectsOnResultColumns:vecProperty
                                                                fromTable:[self indexTableNameOfId:i]
                                                                    where:FTS3MsgContentItem.msg.match([self query])
                                                                   orders:FTS5MsgContentItem.createTime];
                    }
                }break;
                case 2:{
                    WCTProperties vecProperty;
                    vecProperty.push_back(FTS3MsgContentItem.userNameId);
                    vecProperty.push_back(FTS3MsgContentItem.msgLocalId);
                    vecProperty.push_back(FTS3MsgContentItem.createTime);
                    WCDB::Expressions expectedUserNameIds;
                    expectedUserNameIds.push_back(@1);
                    expectedUserNameIds.push_back(@2);
                    expectedUserNameIds.push_back(@3);
                    NSArray* result;
                    for(int i = 0; i<self->m_tableCount; i++){
                        result = [self.database getObjectsOnResultColumns:vecProperty
                                                                fromTable:[self indexTableNameOfId:i]
                                                                    where:FTS3MsgContentItem.userNameId.in(expectedUserNameIds) && FTS3MsgContentItem.createTime < std::numeric_limits<double>::max() / 2 && FTS3MsgContentItem.msg.match([self query])
                                                                   orders:{FTS3MsgContentItem.createTime.asOrder(WCTOrderedDescending)}];
                    }
                }break;
                    
                default:
                    break;
            }
        }];
    }
}

- (void)test_fts5_index_view
{
    [self applyConfig:[self genTestCaseConfigDataType:FTSDataType_FTS5_RowidIndex tableCount:0 needBinary:NO quality:1000000 optimizeLevel:0 querylevel:1 queryTimes:100 queryType:0 needMultiThread:NO]];
    [self setUpDatabase];
    
    [self.database execute:WCDB::StatementCreateVirtualTable().createVirtualTable(@"indexView").usingModule(@"fts5vocab").argument([self indexTableNameOfId:0]).argument(@"instance")];
    WCTColumnsXRows* indexInfo = [self.database getRowsFromStatement:WCDB::StatementSelect().select(WCDB::ResultColumn().all()).from(@"indexView")];
    for(NSArray<WCTValue *> * array in indexInfo){
        NSLog(@"%@, %d, %@, %d", array[0].stringValue, array[1].numberValue.intValue, array[2].stringValue, array[3].numberValue.intValue );
    }
    
    [self tearDownDatabase];
}

- (void)test_delete_rowid
{
    [self applyConfig:[self genTestCaseConfigDataType:FTSDataType_FTS5_RowidIndex tableCount:0 needBinary:NO quality:1000000 optimizeLevel:0 querylevel:1 queryTimes:100 queryType:0 needMultiThread:NO]];
    
    [self measureTest:^(){
        [self.database runTransaction:^BOOL(WCTHandle * _Nonnull) {
            NSArray<FTS5RowidIndexItem*>* items = [self.database getObjectsOfClass:FTS5RowidIndexItem.class fromTable:self.assistTableName where:FTS5RowidIndexItem.userNameId == 1 orders:{FTS5RowidIndexItem.indexRowid.asOrder(WCTOrderedDescending)} limit:100];
            NSLog(@"delete count %lu", (unsigned long)items.count);
            for(FTS5RowidIndexItem* item in items){
                TestCaseAssertTrue([self.database deleteFromTable:self.assistTableName where:FTS5RowidIndexItem.userNameId == item.userNameId && FTS5RowidIndexItem.msgLocalId == item.msgLocalId && FTS5RowidIndexItem.indexRowid == item.indexRowid ]);
                TestCaseAssertTrue([self.database deleteFromTable:[self indexTableNameOfId:0] where:WCDB::Column::rowid() == item.indexRowid]);
            }
            return YES;
        }];
        
    }];
}



@end

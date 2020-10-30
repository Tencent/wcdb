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
    FTSFactory* _factory;
    WCTTable* _table;
    int _queryTimes;
    BOOL _isFts5;
    int _testQuality;
}

- (FTSFactory*)factory
{
    @synchronized(self) {
        if (_factory == nil) {
            _factory = [[FTSFactory alloc] initWithDirectory:self.class.cacheRoot];
        }
        return _factory;
    }
}

- (WCTTable*)table
{
    @synchronized(self) {
        if (_table == nil) {
            if(_isFts5){
                _table = [self.database getTable:self.tableName withClass:FTS5MsgContentItem.class];
            }else{
                _table = [self.database getTable:self.tableName withClass:FTS3MsgContentItem.class];
            }
        }
        return _table;
    }
}

- (void)setUp
{
    [super setUp];
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        [WCTDatabase registerTokenizer:WCDB::FTS3TokenizerModuleTemplate<WCDB::OneOrBinaryTokenizerInfo, WCTOneOrBinaryTokenizer>::specialize()
        named:OneOrBinaryTokenizer_FTS3];
        [WCTDatabase registerTokenizer:WCDB::FTS5TokenizerModuleTemplate<WCTOneOrBinaryTokenizer>::specializeWithContext(nullptr) named:OneOrBinaryTokenizer_FTS5];
    });
    _queryTimes = 1000;
}

- (void)setUpDatabaseWithModule:(NSString*)module
{
    [[Random shared] reset];
    _isFts5 = [module isEqualToString:WCTModuleFTS5];
    self.factory.tolerance = 0.1f;
    self.factory.quality = 1000000;
    self.factory.needOptimize = NO;
    self.factory.autoMergeCount = 3;
    self.factory.isFTS5 = _isFts5;
    if(!_isFts5){
        [self.factory.database addTokenizer:OneOrBinaryTokenizer_FTS3];
        [self.database addTokenizer:OneOrBinaryTokenizer_FTS3];
    }else{
        [self.factory.database addTokenizer:OneOrBinaryTokenizer_FTS5];
        [self.database addTokenizer:OneOrBinaryTokenizer_FTS5];
    }
    [self.factory produce:self.path];
    self.tableName = self.factory.tableName;

    TestCaseAssertOptionalEqual([self.database getNumberOfWalFrames], 0);
    TestCaseAssertFalse([self.database isOpened]);

    WCTHandle* handle = [self.database getHandle];
    [handle prepare:WCDB::StatementInsert().insertIntoTable(self.tableName).columns({WCDB::Column(self.tableName), WCDB::Column(@"rank"), WCDB::Column().rowid()}).values({@"merge", 16, WCDB::BindParameter(1)})];
    [handle bindPointer:(void*)handle toIndex:1];
    [handle step];
    [handle invalidate];
    
}

- (void)tearDownDatabase
{
    [self.database removeFiles];
    _table = nil;
    self.path = nil;
    _factory = nil;
}

- (void)doTestFTSModule:(NSString*)module usingBlock:(void (^)(void))block
{
    [self doMeasure:block setUp:^{
        [self setUpDatabaseWithModule:module];
    } tearDown:^{
        [self tearDownDatabase];
    } checkCorrectness:nil];
}

#pragma mark - Tests

- (void)test_fts3_query
{
    [self doTestFTSModule:WCTModuleFTS3 usingBlock:^(){
        WCTProperties vecProperty;
        vecProperty.push_back(FTS3MsgContentItem.userNameId);
        vecProperty.push_back(FTS3MsgContentItem.msgLocalId);
        vecProperty.push_back(FTS3MsgContentItem.createTime);
        vecProperty.push_back(FTS3MsgContentItem.msg);
        NSArray* result;
        for(int i = 0; i<self->_queryTimes; i++){
            result = [self.table getObjectsOnResultColumns:vecProperty
                                                     where:FTS3MsgContentItem.msg.match(@"该怡是")];
        }
        TestCaseLog(@"result count %d", result.count);
        
    }];
}

- (void)test_fts3_query_extend
{
    [self doTestFTSModule:WCTModuleFTS3 usingBlock:^(){
        UInt32 maxTime = 10000000;
        WCDB::Expressions expectedUserNameIds;
        expectedUserNameIds.push_back(@1);
        expectedUserNameIds.push_back(@2);
        expectedUserNameIds.push_back(@3);

        WCTProperties vecProperty;
        vecProperty.push_back(FTS3MsgContentItem.userNameId);
        vecProperty.push_back(FTS3MsgContentItem.msgLocalId);
        vecProperty.push_back(FTS3MsgContentItem.createTime);
        NSArray* result;
        for(int i = 0; i<self->_queryTimes; i++){
            result = [self.table getObjectsOnResultColumns:vecProperty
                                                     where:FTS3MsgContentItem.userNameId.in(expectedUserNameIds) && FTS3MsgContentItem.createTime < maxTime && FTS3MsgContentItem.msg.match(@"该怡是")
                                                    orders:{FTS3MsgContentItem.createTime.asOrder(WCTOrderedDescending)}];
        }
        TestCaseLog(@"result count %d", result.count);
        
    }];
}

- (void)test_fts5_query
{
    [self doTestFTSModule:WCTModuleFTS5 usingBlock:^(){
        NSArray* result;
        WCTProperties vecProperty;
        vecProperty.push_back(FTS5MsgContentItem.userNameId);
        vecProperty.push_back(FTS5MsgContentItem.msgLocalId);
        vecProperty.push_back(FTS5MsgContentItem.createTime);
        vecProperty.push_back(FTS3MsgContentItem.msg);
        for(int i = 0; i<self->_queryTimes; i++){
            result = [self.table getObjectsOnResultColumns:vecProperty
                                                     where:FTS5MsgContentItem.msg.match(@"该 怡 是")];
        }
        TestCaseLog(@"result count %d", result.count);
    }];
}

- (void)test_fts5_query_extend
{
    [self doTestFTSModule:WCTModuleFTS5 usingBlock:^(){
        UInt32 maxTime = 10000000;
        WCDB::Expressions expectedUserNameIds;
        expectedUserNameIds.push_back(@1);
        expectedUserNameIds.push_back(@2);
        expectedUserNameIds.push_back(@3);

        WCTProperties vecProperty;
        vecProperty.push_back(FTS5MsgContentItem.userNameId);
        vecProperty.push_back(FTS5MsgContentItem.msgLocalId);
        vecProperty.push_back(FTS5MsgContentItem.createTime);
        NSArray* result;
        for(int i = 0; i<self->_queryTimes; i++){
            result = [self.table getObjectsOnResultColumns:vecProperty
                                                     where:FTS5MsgContentItem.userNameId.in(expectedUserNameIds) && FTS5MsgContentItem.createTime < maxTime && FTS5MsgContentItem.msg.match(@"该 怡 是")
                                                    orders:{FTS5MsgContentItem.createTime.asOrder(WCTOrderedDescending)}];
        }
        TestCaseLog(@"result count %d", result.count);
        
    }];
}

- (void)test_fts3_delete_rowid
{
    [self doTestFTSModule:WCTModuleFTS3 usingBlock:^(){
        WCTOneColumn* rowids = [self.table getColumnOnResultColumn:WCDB::Column::rowid() limit:10];
        for(WCTValue* rowid in rowids){
            TestCaseAssertTrue([self.table deleteObjectsWhere:WCDB::Column::rowid() == rowid.numberValue]);
        }
    }];
}

- (void)test_fts5_delete_rowid
{
    [self doTestFTSModule:WCTModuleFTS5 usingBlock:^(){
        WCTOneColumn* rowids = [self.table getColumnOnResultColumn:WCDB::Column::rowid() limit:10];
        for(WCTValue* rowid in rowids){
            TestCaseAssertTrue([self.table deleteObjectsWhere:WCDB::Column::rowid() == rowid.numberValue]);
        }
    }];
}

- (void)test_fts5_mergeIndex
{
    NSArray* testCount = @[@4, @8, @16, @32, @64, @128];
    NSArray* testQuality = @[@1000000];
    for(NSNumber* quality in testQuality){
        for(NSNumber* mergeCount in testCount){
            _testQuality = quality.intValue;
            [self setUpDatabaseWithModule:WCTModuleFTS5];
            
            TestCaseAssertTrue([self.database execute:WCDB::StatementInsert().insertIntoTable(self.tableName).columns({WCDB::Column(self.tableName), WCDB::Column(@"rank")}).values({@"usermerge", 2})]);
            
            WCTHandle* handle = [self.database getHandle];
            
            double totalCost = 0;
            int i = 0;
            TestCaseLog(@"Test merge start: %d\t%d!", quality.intValue, mergeCount.intValue);
            int preTotalChange = 0;
            for(; i < 10; i++){
                NSDate* start = [NSDate date];
                WCDB::StatementInsert merge;
                
                merge.insertIntoTable(self.tableName).columns({WCDB::Column(self.tableName), WCDB::Column(@"rank")}).values({@"merge", mergeCount.intValue});
                
                [handle execute:merge];
                
                if([handle getTotalChanges] - preTotalChange < 2){
                    TestCaseLog(@"Test merge no change: %d\t%d!", quality.intValue, mergeCount.intValue);
                    i--;
                    if(i > 0){
                        TestCaseLog(@"Test merge: %d\t%d\t%.6f", quality.intValue, mergeCount.intValue, totalCost / i);
                    }
                    break;
                }
                
                preTotalChange = [handle getTotalChanges];
                
                totalCost += [[NSDate date] timeIntervalSinceDate:start];
                TestCaseLog(@"Test merge current cost: %.6f", totalCost);
            }
            [handle invalidate];
            [self tearDownDatabase];
            if(i == 10){
                TestCaseLog(@"Test merge: %d\t%d\t%.6f", quality.intValue, mergeCount.intValue, totalCost / 5);
            }
        }
    }
}

//- (void)test_fts3_delete
//{
//    [self doTestFTSModule:WCTModuleFTS3 usingBlock:^(){
//        NSArray<FTS3MsgContentItem*>* itemlist = [self.table getObjectsLimit:10];
//        int i = 0;
//        for(FTS3MsgContentItem* item in itemlist){
//            i++;
//            TestCaseAssertTrue([self.table deleteObjectsWhere:(FTS3MsgContentItem.userNameId == item.userNameId &&
//                                            FTS3MsgContentItem.msgLocalId == item.msgLocalId)]);
//        }
//    }];
//}
//
//- (void)test_fts5_delete
//{
//    [self doTestFTSModule:WCTModuleFTS5 usingBlock:^(){
//        NSArray<FTS5MsgContentItem*>* itemlist = [self.table getObjectsLimit:10];
//        for(FTS5MsgContentItem* item in itemlist){
//            TestCaseAssertTrue([self.table deleteObjectsWhere:(FTS5MsgContentItem.userNameId == item.userNameId &&
//                                                               FTS5MsgContentItem.msgLocalId == item.msgLocalId &&
//                                                               FTS5MsgContentItem.createTime == item.createTime)]);
//        }
//    }];
//}



@end

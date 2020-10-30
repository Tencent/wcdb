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
#import "FTSFactory.h"
#import "FTSData.h"
#import <WCDB/WCDB.h>
#import "TestCaseAssertion.h"
#import "Random+FTSData.h"
#import "WCTDatabase+TestCase.h"

@implementation FTSFactory {
    NSString* _tableName;
    NSDate* m_startProduceTime;
}

- (Class)getORMClass
{
    if(self.isFTS5){
        return FTS5MsgContentItem.class;
    }else{
        return FTS3MsgContentItem.class;
    }
}

- (WCTProperties)getORMProperties
{
    if(self.isFTS5){
        return FTS5MsgContentItem.allProperties;
    }else{
        return FTS3MsgContentItem.allProperties;
    }
}

- (instancetype)initWithDirectory:(NSString*)directory
{
    if (self = [super initWithDirectory:directory]) {
        self.delegate = self;
    }
    return self;
}

- (NSString*)tableName
{
    return @"benchmark";
}

- (void)preparePrototype:(WCTDatabase*)prototype currentQuality:(double)quality
{
    if(quality == 0){
        m_startProduceTime = [NSDate date];
    }
    int maxNumberOfObjects = (int) self.quality;
    int existingNumberOfObjects = (int) quality;
    int numberOfObjects = (int) MAX((self.quality * 0.1), 1);
    if (maxNumberOfObjects - existingNumberOfObjects < numberOfObjects) {
        numberOfObjects = maxNumberOfObjects - existingNumberOfObjects;
    }

    if (existingNumberOfObjects == 0) {
        TestCaseAssertTrue([prototype createVirtualTable:self.tableName withClass:[self getORMClass]]);
    }
    
    for (int needInsert = numberOfObjects; needInsert > 0; needInsert -= 100) {
        NSArray* objects;
        if(self.isFTS5){
            objects = [[Random shared] randomFTS5Items:MIN(needInsert, 100)];
        }else{
            objects = [[Random shared] randomFTS3Items:MIN(needInsert, 100)];
        }
        TestCaseAssertTrue([prototype insertObjects:objects intoTable:self.tableName]);
    }
    
    if(existingNumberOfObjects + numberOfObjects < self.quality * (1.0 - self.tolerance)){
        return;
    }
    
    TestCaseLog(@"FTS%d prepare total time: %.2f", self.isFTS5 ? 5 : 3, [[NSDate date] timeIntervalSinceDate:m_startProduceTime]);
    
    TestCaseAssertTrue([prototype truncateCheckpoint]);
    
    [self tryMergeOrOptimize:prototype];
}

- (void)tryMergeOrOptimize:(WCTDatabase*)prototype
{
    if(self.needOptimize){
        WCDB::StatementInsert optimize;
        optimize.insertIntoTable(self.tableName).column(WCDB::Column(self.tableName)).value(@"optimize");
        
        NSDate* start = [NSDate date];
        TestCaseAssertTrue([prototype execute:optimize]);
        TestCaseLog(@"FTS%d optimize time: %.2f", self.isFTS5 ? 5 : 3, [[NSDate date] timeIntervalSinceDate:start]);
        return;
    }else if(self.autoMergeCount > 0){
        TestCaseAssertTrue([prototype execute:WCDB::StatementInsert().insertIntoTable(self.tableName).columns({WCDB::Column(self.tableName), WCDB::Column(@"rank")}).values({@"usermerge", self.autoMergeCount})]);
        __block int mergeCount = 0;
        NSDate* start = [NSDate date];
        __block int preTotalChange = 0;
        TestCaseAssertTrue([prototype runTransaction:^BOOL(WCTHandle * handle) {
            do{
                preTotalChange = [handle getTotalChanges];
                TestCaseAssertTrue([handle execute:WCDB::StatementInsert().insertIntoTable(self.tableName).columns({WCDB::Column(self.tableName), WCDB::Column(@"rank")}).values({@"merge", 16})]);
                mergeCount++;
            }while([handle getTotalChanges] - preTotalChange > 1);
            return true;
        }]);
        TestCaseLog(@"FTS%d merge time: %.2f, merge count %d", self.isFTS5 ? 5 : 3, [[NSDate date] timeIntervalSinceDate:start], mergeCount);
    }
    
}

- (double)qualityOfPrototype:(WCTDatabase*)prototype
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:prototype.path]) {
        NSDate* start = [NSDate date];
        WCTValue* count = [prototype getValueOnResultColumn:[self getORMProperties].count() fromTable:self.tableName];
        TestCaseLog(@"get quality cost time %.2f", [[NSDate date] timeIntervalSinceDate:start]);
        TestCaseAssertNotNil(count);
        return count.numberValue.intValue;
    }
    return 0;
}

- (NSString*)categoryOfPrototype
{
    NSString* category;
    if(self.isFTS5){
        category = @"FTS5";
    }else{
        category = @"FTS3";
    }
    if(self.needOptimize){
        return [category stringByAppendingString:@"-OPT"];
    }else if(self.autoMergeCount > 0){
        return [category stringByAppendingFormat:@"-%d", self.autoMergeCount];
    }else{
        return category;
    }
}

@end

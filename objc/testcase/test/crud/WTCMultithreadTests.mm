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

#import "WTCCRUDTestCase.h"
#import <thread>
#import <algorithm>

@interface WTCMultithreadTests : WTCCRUDTestCase
@property(nonatomic, readonly) int expectedCount;
@property(nonatomic, readonly) NSArray<WTCCRUDObject*>* preInsertedMultithreadObjects;
@end

@implementation WTCMultithreadTests {
    dispatch_group_t _group;
    dispatch_queue_t _queue;
}

- (void)setUp {
    [super setUp];
    
    _expectedCount = 10000;
    
    NSMutableArray* preInsertedMultithreadObjects = [[NSMutableArray alloc] initWithCapacity:_expectedCount];
    for (int i = 0; i < _expectedCount; ++i) {
        WTCCRUDObject* object = [[WTCCRUDObject alloc] init];
        object.isAutoIncrement = YES;
        [preInsertedMultithreadObjects addObject:object];
    }
    _preInsertedMultithreadObjects = preInsertedMultithreadObjects;
    
    _group = dispatch_group_create();
    _queue = dispatch_queue_create([NSString stringWithFormat:@"%d", self.recommendedTag].UTF8String, DISPATCH_QUEUE_CONCURRENT);
    
    XCTAssertTrue([self.database deleteObjectsFromTable:WTCCRUDObject.Name]);
    [self.database close];
}

- (void)insertObjectWithExpectedCount
{
    XCTAssertTrue([self.database insertObjects:self.preInsertedMultithreadObjects into:WTCCRUDObject.Name]);
}

+ (NSTimeInterval)tick:(void(^)(void))block
{
    NSTimeInterval before = [NSProcessInfo processInfo].systemUptime;
    block();
    NSTimeInterval after = [NSProcessInfo processInfo].systemUptime;  
    return after - before;
}

- (void)testMultithreadReadRead {
    //Give
    [self insertObjectWithExpectedCount];
    [self.database close];
    
    __block NSArray<WTCCRUDObject*>* singleReadResults = nil;
    NSTimeInterval singleReadCost = [WTCMultithreadTests tick:^{
        dispatch_group_async(_group, _queue, ^{
            singleReadResults = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
        });
        dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    }];
    XCTAssertNotNil(singleReadResults);
    XCTAssertEqual(singleReadResults.count, self.expectedCount);
    singleReadResults = nil;
    [self.database close];
    
    //When
    __block NSArray<WTCCRUDObject*>* readReadResults1 = nil;
    __block NSArray<WTCCRUDObject*>* readReadResults2 = nil;
    NSTimeInterval readReadCost = [WTCMultithreadTests tick:^{
        dispatch_group_async(_group, _queue, ^{
            readReadResults1 = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
        });
        dispatch_group_async(_group, _queue, ^{
            readReadResults2 = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
        });
        dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    }];
    XCTAssertNotNil(readReadResults1);
    XCTAssertEqual(readReadResults1.count, self.expectedCount);
    XCTAssertNotNil(readReadResults2);
    XCTAssertEqual(readReadResults2.count, self.expectedCount);
    readReadResults1 = nil;
    readReadResults2 = nil;
    //Then
#if DEBUG
#else
    XCTAssertLessThanOrEqual(readReadCost, (singleReadCost * 2) * 0.9);
#endif
}

- (void)testMultithreadReadWrite {
    //Give
    NSTimeInterval singleWriteCost = [WTCMultithreadTests tick:^{
        dispatch_group_async(_group, _queue, ^{
            [self insertObjectWithExpectedCount];
        });
        dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    }];
    [self.database close];
    
    //Get single read time
    __block NSArray<WTCCRUDObject*>* singleReadResults = nil;
    NSTimeInterval singleReadCost = [WTCMultithreadTests tick:^{
        singleReadResults = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    }];
    XCTAssertNotNil(singleReadResults);
    XCTAssertEqual(singleReadResults.count, self.expectedCount);
    singleReadResults = nil;
    [self.database close];
    
    //When
    __block NSArray<WTCCRUDObject*>* readWriteResults = nil;
    NSTimeInterval readWriteCost = [WTCMultithreadTests tick:^{
        dispatch_group_async(_group, _queue, ^{
            [self insertObjectWithExpectedCount];
        });
        dispatch_group_async(_group, _queue, ^{
            readWriteResults = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
        });
        dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    }];
    XCTAssertNotNil(readWriteResults);
    XCTAssertEqual(readWriteResults.count, self.expectedCount);
    readWriteResults = nil;
    //Then
#if DEBUG
#else
    XCTAssertLessThanOrEqual(readWriteCost, (singleReadCost + singleWriteCost) * 0.9);
#endif
}

- (void)testMultithreadWriteWrite {
    //Give
    NSTimeInterval singleWriteCost = [WTCMultithreadTests tick:^{
        dispatch_group_async(_group, _queue, ^{
            [self insertObjectWithExpectedCount];
        });
        dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    }];
    [self.database close];
    
    //Get single read time
    XCTAssertTrue([self.database deleteObjectsFromTable:WTCCRUDObject.Name]);
    [self.database close];
    //When
    NSTimeInterval writeWriteCost = [WTCMultithreadTests tick:^{
        dispatch_group_async(_group, _queue, ^{
            [self insertObjectWithExpectedCount];
        });
        dispatch_group_async(_group, _queue, ^{
            [self insertObjectWithExpectedCount];
        });
        dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    }];
#if DEBUG
#else
    XCTAssertLessThanOrEqual(writeWriteCost, (singleWriteCost * 2) * 1.2);
#endif
}

- (void)testConcurrencyExceeds
{
    int maxConcurrency = std::max<int>(std::thread::hardware_concurrency(), 64);
    __block int currentConcurrency = 0;
    NSCondition* cond = [[NSCondition alloc] init];
    for (int i = 0; i < maxConcurrency; ++i) {
        dispatch_group_async(_group, _queue, ^{
            [cond lock];
            
            currentConcurrency += 1;
            
            XCTAssertTrue([self.database beginWithMode:WCTTransactionModeDeferred]);
            
            [cond wait];
            
            currentConcurrency -= 1;
            
            [cond unlock];
        });
    }
    
    while (YES) {
        [cond lock];
        
        if (currentConcurrency == maxConcurrency) {
            [cond unlock];
            break;
        }else {
            [cond unlock];
            continue;
        }        
    }
    
    XCTAssertEqual(currentConcurrency, maxConcurrency);
    
    XCTAssertFalse([self.database beginWithMode:WCTTransactionModeDeferred]);
    
    [cond broadcast];
    
    dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    
    XCTAssertEqual(currentConcurrency, 0);
}

@end

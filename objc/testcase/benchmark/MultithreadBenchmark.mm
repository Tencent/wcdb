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

#import "BenchmarkCommon.h"

@interface MultithreadBenchmark : Benchmark

@end

@implementation MultithreadBenchmark {
    dispatch_group_t _group;
    dispatch_queue_t _queue;
}

- (void)setUp
{
    [super setUp];
    _group = dispatch_group_create();
    _queue = dispatch_queue_create(self.className.UTF8String, DISPATCH_QUEUE_CONCURRENT);
}

- (void)test_read_read
{
    NSString *tableName = [self getTableNameWithIndex:0];
    __block NSArray<BenchmarkObject *> *results1 = nil;
    __block NSArray<BenchmarkObject *> *results2 = nil;

    [self
    measure:^{
        dispatch_group_async(_group, _queue, ^{
            results1 = [self.database getObjectsOfClass:BenchmarkObject.class fromTable:tableName orderBy:BenchmarkObject.key.asOrder(WCTOrderedAscending)];
        });
        dispatch_group_async(_group, _queue, ^{
            results2 = [self.database getObjectsOfClass:BenchmarkObject.class fromTable:tableName orderBy:BenchmarkObject.key.asOrder(WCTOrderedAscending)];
        });
        dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    }
    setUp:^{
        [self setUpDatabase];
        [self setUpWithPreCreateTable:1];
        [self setUpWithPreCreateObject:self.config.readCount];
        [self setUpWithPreInsertObjects:self.objects intoTable:tableName];

        [self tearDownDatabaseCache];
        [self setUpDatabaseCache];
    }
    tearDown:^{
        [self tearDownDatabase];
    }
    checkCorrectness:^{
        XCTAssertEqual(results1.count, self.config.readCount);
        XCTAssertTrue([results1 isEqualToBenchmarkObjects:self.objects]);
        XCTAssertEqual(results2.count, self.config.readCount);
        XCTAssertTrue([results2 isEqualToBenchmarkObjects:self.objects]);
    }];
}

- (void)test_read_write
{
    NSString *tableName = [self getTableNameWithIndex:0];
    __block BOOL result1 = NO;
    __block NSArray<BenchmarkObject *> *results2 = nil;
    [self
    measure:^{
        dispatch_group_async(_group, _queue, ^{
            result1 = [self.database insertObjects:self.objects intoTable:tableName];
        });
        dispatch_group_async(_group, _queue, ^{
            results2 = [self.database getObjectsOfClass:BenchmarkObject.class fromTable:tableName orderBy:BenchmarkObject.key.asOrder(WCTOrderedAscending)];
        });
        dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    }
    setUp:^{
        [self setUpDatabase];
        [self setUpWithPreCreateTable:1];
        [self setUpWithPreCreateObject:self.config.batchWriteCount];
        [self setUpWithPreInsertObjects:self.objects intoTable:tableName];

        [self tearDownDatabaseCache];
        [self setUpDatabaseCache];
    }
    tearDown:^{
        [self tearDownDatabase];
    }
    checkCorrectness:^{
        NSArray<BenchmarkObject *> *objects = [self.database getObjectsOfClass:BenchmarkObject.class fromTable:tableName orderBy:BenchmarkObject.key.asOrder(WCTOrderedAscending)];
        XCTAssertEqual(objects.count, self.config.batchWriteCount * 2);
        for (NSInteger i = 0; i < objects.count; ++i) {
            XCTAssertTrue([objects[i] isEqualToBenchmarkObject:self.objects[i / 2]]);
        }
    }];
}

- (void)test_write_write
{
    NSString *tableName = [self getTableNameWithIndex:0];
    __block BOOL result1 = NO;
    __block BOOL result2 = NO;
    [self
    measure:^{
        dispatch_group_async(_group, _queue, ^{
            result1 = [self.database insertObjects:self.objects intoTable:tableName];
        });
        dispatch_group_async(_group, _queue, ^{
            result2 = [self.database insertObjects:self.objects intoTable:tableName];
        });
        dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    }
    setUp:^{
        [self setUpDatabase];
        [self setUpWithPreCreateTable:1];
        [self setUpWithPreCreateObject:self.config.batchWriteCount];

        [self tearDownDatabaseCache];
        [self setUpDatabaseCache];
    }
    tearDown:^{
        [self tearDownDatabase];
    }
    checkCorrectness:^{
        NSArray<BenchmarkObject *> *objects = [self.database getObjectsOfClass:BenchmarkObject.class fromTable:tableName orderBy:BenchmarkObject.key.asOrder(WCTOrderedAscending)];
        XCTAssertEqual(objects.count, self.config.batchWriteCount * 2);
        for (NSInteger i = 0; i < objects.count; ++i) {
            XCTAssertTrue([objects[i] isEqualToBenchmarkObject:self.objects[i / 2]]);
        }
    }];
}

@end

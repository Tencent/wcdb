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

#import "BaselineBenchmark.h"
#import "StableRandom.h"

@implementation BaselineBenchmark

- (void)setUp
{
    [super setUp];
    self.factory.tolerance = 0.0f;
    self.destination = self.path;
}

- (void)doTestWrite
{
    int numberOfObjects = 10000;

    NSMutableArray* objects = [NSMutableArray arrayWithCapacity:numberOfObjects];
    Random* random = [[StableRandom alloc] init];
    random.seed = 0;
    for (int i = 0; i < numberOfObjects; ++i) {
        BenchmarkObject* object = [[BenchmarkObject alloc] init];
        object.identifier = i;
        object.content = random.data;
        [objects addObject:object];
    }

    __block BOOL result;
    [self
    measure:^{
        for (BenchmarkObject* object in objects) {
            if (![self.database insertObject:object intoTable:@"testTable"]) {
                result = NO;
                return;
            }
        }
        result = YES;
    }
    setUp:^{
        [self.factory setProductionLineObjects:0];
        TestCaseAssertTrue([self.factory production:self.destination]);
    }
    tearDown:^{
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)doTestRead
{
    int numberOfObjects = 1000000;
    NSString* tableName = @"testTable";

    __block NSArray<BenchmarkObject*>* result;
    [self
    measure:^{
        result = [self.database getObjectsOfClass:BenchmarkObject.class fromTable:tableName];
    }
    setUp:^{
        [self.factory setProductionLineObjects:numberOfObjects];
        TestCaseAssertTrue([self.factory production:self.destination]);
        TestCaseAssertTrue([self.database createTableAndIndexes:@"testTable" withClass:BenchmarkObject.class]);
    }
    tearDown:^{
        result = nil;
    }
    checkCorrectness:^{
        TestCaseAssertEqual(result.count, numberOfObjects);
    }];
}

- (void)doTestBatchWrite
{
    int numberOfObjects = 1000000;

    NSMutableArray* objects = [NSMutableArray arrayWithCapacity:numberOfObjects];
    Random* random = [[StableRandom alloc] init];
    random.seed = 0;
    for (int i = 0; i < numberOfObjects; ++i) {
        BenchmarkObject* object = [[BenchmarkObject alloc] init];
        object.identifier = i;
        object.content = random.data;
        [objects addObject:object];
    }

    __block BOOL result;
    [self
    measure:^{
        result = [self.database insertObjects:objects intoTable:@"testTable"];
    }
    setUp:^{
        [self.factory setProductionLineObjects:0];
        TestCaseAssertTrue([self.factory production:self.destination]);
        TestCaseAssertTrue([self.database createTableAndIndexes:@"testTable" withClass:BenchmarkObject.class]);
    }
    tearDown:^{
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)test_write
{
    [self doTestWrite];
}

- (void)test_read
{
    [self doTestRead];
}

- (void)test_batch_write
{
    [self doTestBatchWrite];
}

@end

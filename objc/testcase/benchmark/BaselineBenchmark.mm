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

@implementation BaselineBenchmark {
    NSString* _tableName;
}

- (void)setUp
{
    [super setUp];
    self.factory.tolerance = 0.0f;
    self.factory.expectedQuality = 1000000;
}

- (void)setUpDatabase
{
    NSString* path = [self.factory production:self.directory];
    TestCaseAssertTrue(path != nil);
    self.path = path;

    [self doSetUpDatabase];

    [self.database close]; // reset cache
    TestCaseAssertTrue([self.database canOpen]);
}

- (void)tearDownDatabase
{
    [self.database removeFiles];
    [self doTearDownDatabase];
}

- (void)doSetUpDatabase
{
}

- (void)doTearDownDatabase
{
}

- (NSString*)tableName
{
    if (!_tableName) {
        _tableName = [NSString stringWithFormat:@"t_%@", self.random.string];
    }
    return _tableName;
}

- (void)doTestWrite
{
    int numberOfObjects = 10000;

    __block NSMutableArray* objects = nil;
    __block BOOL result;
    [self
    doMeasure:^{
        for (BenchmarkObject* object in objects) {
            if (![self.database insertObject:object intoTable:self.tableName]) {
                result = NO;
                return;
            }
        }
        result = YES;
    }
    setUp:^{
        [self setUpDatabase];

        if (objects == nil) {
            objects = [NSMutableArray arrayWithCapacity:numberOfObjects];
            for (int i = self.factory.expectedQuality; i < self.factory.expectedQuality + numberOfObjects; ++i) {
                BenchmarkObject* object = [[BenchmarkObject alloc] init];
                object.identifier = i;
                object.content = self.random.data;
                [objects addObject:object];
            }
        }
    }
    tearDown:^{
        [self tearDownDatabase];
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)doTestRead
{
    __block NSArray<BenchmarkObject*>* result;
    [self
    doMeasure:^{
        result = [self.database getObjectsOfClass:BenchmarkObject.class fromTable:self.tableName];
    }
    setUp:^{
        [self setUpDatabase];
    }
    tearDown:^{
        [self tearDownDatabase];
        result = nil;
    }
    checkCorrectness:^{
        TestCaseAssertEqual(result.count, self.factory.expectedQuality);
    }];
}

- (void)doTestBatchWrite
{
    int numberOfObjects = 1000000;

    __block NSMutableArray* objects = nil;
    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database insertObjects:objects intoTable:self.tableName];
    }
    setUp:^{
        [self setUpDatabase];

        if (objects == nil) {
            objects = [NSMutableArray arrayWithCapacity:numberOfObjects];
            for (int i = self.factory.expectedQuality; i < self.factory.expectedQuality + numberOfObjects; ++i) {
                BenchmarkObject* object = [[BenchmarkObject alloc] init];
                object.identifier = i;
                object.content = self.random.data;
                [objects addObject:object];
            }
        }
    }
    tearDown:^{
        [self tearDownDatabase];
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

- (void)test_create_index
{
    WCDB::StatementCreateIndex statement = WCDB::StatementCreateIndex().createIndex(@"testTable_index").table(@"testTable").indexed(BenchmarkObject.identifier);

    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database execute:statement];
    }
    setUp:^{
        [self setUpDatabase];
    }
    tearDown:^{
        [self tearDownDatabase];
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

#pragma mark - ReusableFactoryPreparation
- (BOOL)stepPreparePrototype:(NSString*)path
{
    int numberOfObjects = [self getQuality:path];
    int maxNumberOfObjects = self.factory.expectedQuality;
    int step = maxNumberOfObjects / 100;
    if (step > maxNumberOfObjects - numberOfObjects) {
        step = maxNumberOfObjects - numberOfObjects;
    }
    if (step < 1) {
        step = 1;
    }

    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];

    int start = 0;
    BOOL create = NO;
    if ([database tableExists:self.tableName]) {
        start = [database getValueFromStatement:WCDB::StatementSelect().select(BenchmarkObject.identifier.count()).from(self.tableName)].numberValue.intValue;
    } else {
        create = YES;
    }
    NSMutableArray* objects = [NSMutableArray arrayWithCapacity:step];
    for (int i = start; i < start + step; ++i) {
        BenchmarkObject* object = [[BenchmarkObject alloc] init];
        object.identifier = i;
        object.content = self.random.data;
        [objects addObject:object];
    }

    BOOL committed = [database runTransaction:^BOOL(WCTHandle* handle) {
        if (create) {
            TestCaseAssertTrue([database createTableAndIndexes:self.tableName withClass:BenchmarkObject.class]);
        }
        TestCaseAssertTrue([handle insertObjects:objects intoTable:self.tableName]);
        return YES;
    }];
    TestCaseAssertTrue(committed);
    TestCaseAssertTrue([database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).with("TRUNCATE")]);
    return YES;
}

- (double)getQuality:(NSString*)path
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    if ([database tableExists:self.tableName]) {
        return [database getValueFromStatement:WCDB::StatementSelect().select(BenchmarkObject.allProperties.count()).from(self.tableName)].numberValue.doubleValue;
    }
    return 0;
}

- (NSString*)category
{
    return @"Baseline";
}

@end

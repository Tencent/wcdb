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

#import "ObjectsBasedBenchmark.h"

@implementation ObjectsBasedBenchmark {
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
    __block NSString* path;
    [self.database close:^{
        TestCaseAssertTrue([self.database removeFiles]);
        path = [self.factory production:self.directory];
    }];
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
    @synchronized(self) {
        if (_tableName == nil) {
            _tableName = @"benchmark";
        }
        return _tableName;
    }
}

- (void)doTestWrite
{
    int numberOfObjects = 10000;

    __block NSArray* objects = nil;
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
            objects = [self.random benchmarkObjectsWithCount:numberOfObjects startingFromIdentifier:(int) self.factory.expectedQuality];
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

    __block NSArray* objects = nil;
    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database insertObjects:objects intoTable:self.tableName];
    }
    setUp:^{
        [self setUpDatabase];

        if (objects == nil) {
            objects = [self.random benchmarkObjectsWithCount:numberOfObjects startingFromIdentifier:(int) self.factory.expectedQuality];
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

- (void)doTestCreateIndex
{
    NSString* indexName = [NSString stringWithFormat:@"%@_index", self.tableName];
    WCDB::StatementCreateIndex statement = WCDB::StatementCreateIndex().createIndex(indexName).table(self.tableName).indexed(BenchmarkObject.identifier);

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

- (void)doTestVacuum
{
    WCDB::StatementVacuum statement = WCDB::StatementVacuum().vacuum();
    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database execute:statement];
    }
    setUp:^{
        [self setUpDatabase];
        TestCaseAssertTrue([self.database deleteFromTable:self.tableName limit:(int) self.factory.expectedQuality / 10]);
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
- (BOOL)willEndPreparing:(NSString*)path
{
    return [[[WCTDatabase alloc] initWithPath:path] truncateCheckpoint];
}

- (BOOL)stepPreparePrototype:(NSString*)path
{
    int numberOfObjects = (int) [self getQuality:path];
    int maxNumberOfObjects = (int) self.factory.expectedQuality;
    int step = maxNumberOfObjects / 100;
    if (step > maxNumberOfObjects - numberOfObjects) {
        step = maxNumberOfObjects - numberOfObjects;
    }

    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];

    int start = 0;
    BOOL create = NO;
    auto optionalExists = [database tableExists:self.tableName];
    TestCaseAssertFalse(optionalExists.failed());
    if (optionalExists.value()) {
        start = [database getValueFromStatement:WCDB::StatementSelect().select(BenchmarkObject.identifier.count()).from(self.tableName)].numberValue.intValue;
    } else {
        create = YES;
    }
    NSArray* objects = [self.random benchmarkObjectsWithCount:step startingFromIdentifier:start];

    BOOL committed = [database runTransaction:^BOOL(WCTHandle* handle) {
        if (create) {
            TestCaseAssertTrue([database createTable:self.tableName withClass:BenchmarkObject.class]);
        }
        TestCaseAssertTrue([handle insertObjects:objects intoTable:self.tableName]);
        return YES;
    }];
    TestCaseAssertTrue(committed);
    TestCaseAssertTrue([database truncateCheckpoint]);
    return YES;
}

- (double)getQuality:(NSString*)path
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    auto exists = [database tableExists:self.tableName];
    TestCaseAssertFalse(exists.failed());
    double quality = 0;
    if (exists) {
        NSNumber* nsQuality = [database getValueFromStatement:WCDB::StatementSelect().select(BenchmarkObject.allProperties.count()).from(self.tableName)].numberValue;
        TestCaseAssertTrue(nsQuality != nil);
        quality = nsQuality.doubleValue;
    }
    return quality;
}

- (NSString*)category
{
    return @"Objects";
}

@end

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

#import "TestCase.h"

@interface TableBenchmark : Benchmark

@end

@implementation TableBenchmark

- (void)setUp
{
    [super setUp];
    self.factory.tolerance = 0.0f;
    self.factory.expectedQuality = 100000;
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

    [self.database close];
    TestCaseAssertTrue([self.database canOpen]);
}

- (void)tearDownDatabase
{
    [self.database removeFiles];
}

- (void)test_initialization
{
    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database canOpen];
    }
    setUp:^{
        [self setUpDatabase];
        [self.database close]; // reset cache
    }
    tearDown:^{
        [self tearDownDatabase];
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)test_exists
{
    __block NSArray<NSString*>* tableNames;
    __block BOOL result;
    [self
    doMeasure:^{
        result = YES;
        for (NSString* tableName in tableNames) {
            if (![self.database tableExists:tableName]) {
                result = NO;
                break;
            }
        }
    }
    setUp:^{
        [self setUpDatabase];
        if (tableNames == nil) {
            NSString* pattern = [NSString stringWithFormat:@"%s%%", WCDB::Syntax::builtinTablePrefix];

            NSMutableArray<NSString*>* names = [NSMutableArray arrayWithCapacity:(NSUInteger) self.factory.expectedQuality];
            for (WCTValue* value in [self.database getColumnFromStatement:WCDB::StatementSelect().select(WCTMaster.name).from(WCTMaster.tableName).where(WCTMaster.name.notLike(pattern))]) {
                [names addObject:value.stringValue];
            }
            tableNames = [NSArray arrayWithArray:names];
        }
        TestCaseAssertTrue(tableNames.count == self.factory.expectedQuality);
    }
    tearDown:^{
        [self tearDownDatabase];
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)test_insert_into_multiple_tables_within_one_transaction
{
    int numberOfObjects = 10000;
    int numberOfTables = 100;
    NSString* pattern = [NSString stringWithFormat:@"%s%%", WCDB::Syntax::builtinTablePrefix];
    __block NSArray<NSString*>* tableNames = nil;

    __block NSArray* objects = nil;
    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database runTransaction:^BOOL(WCTHandle* handle) {
            for (NSString* tableName in tableNames) {
                if (![handle insertObjects:objects intoTable:tableName]) {
                    return NO;
                }
            }
            return YES;
        }];
    }
    setUp:^{
        [self setUpDatabase];

        tableNames = (NSArray<NSString*>*) [self.database getColumnFromStatement:WCDB::StatementSelect().select(WCTMaster.name).from(WCTMaster.tableName).where(WCTMaster.name.notLike(pattern)).limit(numberOfTables)];
        TestCaseAssertEqual(tableNames.count, numberOfTables);

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

- (void)test_insert_into_multiple_tables_within_seperated_transaction
{
    int numberOfObjects = 10000;
    int numberOfTables = 100;
    NSString* pattern = [NSString stringWithFormat:@"%s%%", WCDB::Syntax::builtinTablePrefix];
    __block NSArray<NSString*>* tableNames = nil;

    __block NSArray* objects = nil;
    __block BOOL result;
    [self
    doMeasure:^{
        for (NSString* tableName in tableNames) {
            if (![self.database insertObjects:objects intoTable:tableName]) {
                result = NO;
                return;
            }
        }
        result = YES;
    }
    setUp:^{
        [self setUpDatabase];

        tableNames = (NSArray<NSString*>*) [self.database getColumnFromStatement:WCDB::StatementSelect().select(WCTMaster.name).from(WCTMaster.tableName).where(WCTMaster.name.notLike(pattern)).limit(numberOfTables)];
        TestCaseAssertEqual(tableNames.count, numberOfTables);

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

- (void)test_create_tables
{
    int numberOfTables = 100;
    __block NSArray* tableNames = nil;
    __block BOOL result;
    [self
    doMeasure:^{
        for (NSString* tableName in tableNames) {
            if (![self.database createTable:tableName withClass:BenchmarkObject.class]) {
                result = NO;
                return;
            }
        }
        result = YES;
    }
    setUp:^{
        [self setUpDatabase];

        if (tableNames == nil) {
            tableNames = [NSArray arrayWithArray:[self.random tableNamesWithCount:numberOfTables]];
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

#pragma mark - ReusableFactoryPreparation
- (BOOL)stepPreparePrototype:(NSString*)path
{
    int numberOfTables = (int) [self getQuality:path];
    int maxNumberOfTables = (int) self.factory.expectedQuality;
    int step = maxNumberOfTables / 100;
    if (step > maxNumberOfTables - numberOfTables) {
        step = maxNumberOfTables - numberOfTables;
    }
    if (step < 1) {
        step = 1;
    }

    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    return [database runTransaction:^BOOL(WCTHandle* handle) {
               WCDB_UNUSED(handle)
               for (int i = 0; i < step; ++i) {
                   if (![database createTable:self.random.tableName withClass:BenchmarkObject.class]) {
                       return NO;
                   }
               }
               return YES;
           }]
           && [database truncateCheckpoint];
}

- (double)getQuality:(NSString*)path
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    NSNumber* quality = [database getValueFromStatement:WCDB::StatementSelect().select(WCTMaster.allProperties.count()).from(WCTMaster.tableName).where(WCTMaster.type == @"table")].numberValue;
    TestCaseAssertTrue(quality != nil);
    return quality.doubleValue;
}

- (NSString*)category
{
    return @"Tables";
}

@end

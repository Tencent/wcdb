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

#import <TestCase/TestCase.h>

@interface InitializationBenchmark : Benchmark

@end

@implementation InitializationBenchmark

- (void)setUp
{
    [super setUp];
    self.factory.tolerance = 0.0f;
    self.factory.expectedQuality = 100000;
}

- (void)setUpDatabase
{
    NSString* path = [self.factory production:self.directory];
    TestCaseAssertTrue(path != nil);
    self.path = path;

    [self.database close]; // reset cache
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
    int numberOfTables = [self getQuality:path];
    int maxNumberOfTables = self.factory.expectedQuality;
    int step = maxNumberOfTables / 100;
    if (step > maxNumberOfTables - numberOfTables) {
        step = maxNumberOfTables - numberOfTables;
    }
    if (step < 1) {
        step = 1;
    }

    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    return [database runTransaction:^BOOL(WCTHandle* handle) {
               for (int i = 0; i < step; ++i) {
                   if (![database createTableAndIndexes:self.random.tableName withClass:BenchmarkObject.class]) {
                       return NO;
                   }
               }
               return YES;
           }]
           && [database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).with("TRUNCATE")];
}

- (double)getQuality:(NSString*)path
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    return [database getValueFromStatement:WCDB::StatementSelect().select(WCTMaster.allProperties.count()).from(WCTMaster.tableName).where(WCTMaster.type == @"table")].numberValue.doubleValue;
}

- (NSString*)category
{
    return @"Tables";
}

@end

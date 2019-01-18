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

@interface RepairBenchmark : Benchmark
@property (nonatomic, readonly) int step;
@end

@implementation RepairBenchmark

- (void)setUp
{
    [super setUp];

    self.factory.expectedQuality = 100.0; // file size in MB
    _step = self.factory.expectedQuality * 100;
    self.factory.tolerance = 0.02;
}

- (void)setUpDatabase
{
    NSString* path = [self.factory production:self.directory];
    TestCaseAssertTrue(path != nil);
    self.path = path;
}

- (void)tearDownDatabase
{
    TestCaseAssertTrue([self.database removeFiles]);
}

- (void)test_backup
{
    __block BOOL result;
    [self
    doMeasure:^{
        result = [self.database backup];
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
        TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.firstMaterialPath]);
    }];
}

- (void)test_repair
{
    __block double score;
    [self
    doMeasure:^{
        score = [self.database retrieve:nil];
    }
    setUp:^{
        [self setUpDatabase];
        TestCaseAssertTrue([self.database backup]);
    }
    tearDown:^{
        [self tearDownDatabase];
        score = 0.0f;
    }
    checkCorrectness:^{
        TestCaseAssertEqual(score, 1.0f);
    }];
}

- (void)test_repair_without_backup
{
    __block double score;
    [self
    doMeasure:^{
        score = [self.database retrieve:nil];
    }
    setUp:^{
        [self setUpDatabase];
    }
    tearDown:^{
        [self tearDownDatabase];
        score = 0.0f;
    }
    checkCorrectness:^{
        TestCaseAssertEqual(score, 1.0f);
    }];
}

#pragma mark - ReusableFactoryPreparation
- (BOOL)stepPreparePrototype:(NSString*)path
{
    NSArray* objects = [self.random benchmarkObjectsWithCount:self.step];

    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    return [database runTransaction:^BOOL(WCTHandle* handle) {
               NSString* tableName = self.random.tableName;
               return [database createTableAndIndexes:tableName withClass:BenchmarkObject.class]
                      && [handle insertObjects:objects intoTable:tableName];
           }]
           && [database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).with("TRUNCATE")];
}

- (double)getQuality:(NSString*)path
{
    NSUInteger fileSize = [[[WCTDatabase alloc] initWithPath:path] getFilesSize].value();
    return (double) fileSize / 1024 / 1024;
}

- (NSString*)category
{
    return @"Repair";
}

@end

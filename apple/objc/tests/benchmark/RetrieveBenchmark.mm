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

@interface RetrieveBenchmark : Benchmark
@property (nonatomic, readonly) int step;
@end

@implementation RetrieveBenchmark

- (void)setUp
{
    [super setUp];

    self.factory.quality = 100.0; // file size in MB
    _step = (int) self.factory.quality * 100;
    self.factory.tolerance = 0.02;
}

- (void)setUpDatabase
{
    __block NSString* path;
    [self.database close:^{
        TestCaseAssertTrue([self.database removeFiles]);
        path = [self.factory produce:self.directory];
    }];
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
        TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    }];
}

- (void)test_retrieve
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

- (void)test_retrieve_without_backup
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

#pragma mark - FactoryPreparation
- (BOOL)stepPreparePrototype:(NSString*)path
{
    NSArray* objects = [self.random testCaseObjectsWithCount:self.step startingFromIdentifier:0];

    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    return [database runTransaction:^BOOL(WCTHandle* handle) {
               NSString* tableName = self.random.tableName;
               return [database createTable:tableName withClass:TestCaseObject.class]
                      && [handle insertObjects:objects intoTable:tableName];
           }]
           && [database truncateCheckpoint];
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

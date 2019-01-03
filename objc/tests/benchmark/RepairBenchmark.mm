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

#import "Benchmark.h"

@interface RepairBenchmark : Benchmark
@property (nonatomic, readonly) NSString* firstMaterial;
@property (nonatomic, readonly) NSString* lastMaterial;
@end

@implementation RepairBenchmark

- (void)setUp
{
    [super setUp];

    self.factory.tolerance = 0.02;
    _firstMaterial = [self.database.path stringByAppendingString:@"-first.material"];
    _lastMaterial = [self.database.path stringByAppendingString:@"-last.material"];
}

- (void)test_backup
{
    __block BOOL result;
    [self
    measure:^{
        result = [self.database backup];
    }
    setUp:^{
        [self.factory setProductionLineFileSizeInMB:100];
        TestCaseAssertTrue([self.factory production:self.path]);
    }
    tearDown:^{
        if ([self.fileManager fileExistsAtPath:self.firstMaterial]) {
            TestCaseAssertTrue([self.fileManager removeItemAtPath:self.firstMaterial error:nil]);
        }
        if ([self.fileManager fileExistsAtPath:self.lastMaterial]) {
            TestCaseAssertTrue([self.fileManager removeItemAtPath:self.lastMaterial error:nil]);
        }
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
        TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.firstMaterial]);
    }];
}

- (void)test_repair
{
    __block double score;
    [self
    measure:^{
        score = [self.database retrieve:nil];
    }
    setUp:^{
        [self.factory setProductionLineFileSizeInMB:100];
        TestCaseAssertTrue([self.factory production:self.path]);
        TestCaseAssertTrue([self.database backup]);
    }
    tearDown:^{
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
    measure:^{
        score = [self.database retrieve:nil];
    }
    setUp:^{
        [self.factory setProductionLineFileSizeInMB:100];
        TestCaseAssertTrue([self.factory production:self.path]);
    }
    tearDown:^{
        score = 0.0f;
    }
    checkCorrectness:^{
        TestCaseAssertEqual(score, 1.0f);
    }];
}

@end

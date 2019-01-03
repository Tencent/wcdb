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

    _firstMaterial = [self.database.path stringByAppendingString:@"-first.material"];
    _lastMaterial = [self.database.path stringByAppendingString:@"-last.material"];

    [self.factory setProductionLineFileSizeInMB:100];
}

- (void)test_backup
{
    [self
    measure:^{
        TestCaseAssertTrue([self.database backup]);
    }
    setUp:^{
        TestCaseAssertTrue([self.factory production:self.path]);
    }
    tearDown:^{
        if ([self.fileManager fileExistsAtPath:self.firstMaterial]) {
            TestCaseAssertTrue([self.fileManager removeItemAtPath:self.firstMaterial error:nil]);
        }
        if ([self.fileManager fileExistsAtPath:self.lastMaterial]) {
            TestCaseAssertTrue([self.fileManager removeItemAtPath:self.lastMaterial error:nil]);
        }
    }
    checkCorrectness:^{
        XCTAssertTrue([self.fileManager fileExistsAtPath:self.firstMaterial]);
    }];
}

- (void)test_repair
{
    [self
    measure:^{
        TestCaseAssertTrue([self.database retrieve:nil] == 1.0f);
    }
    setUp:^{
        TestCaseAssertTrue([self.factory production:self.path]);
        TestCaseAssertTrue([self.database backup]);
    }
    tearDown:^{
    }
    checkCorrectness:^{
    }];
}

- (void)test_repair_without_backup
{
    [self
    measure:^{
        TestCaseAssertTrue([self.database retrieve:nil] == 1.0f);
    }
    setUp:^{
        TestCaseAssertTrue([self.factory production:self.path]);
    }
    tearDown:^{
    }
    checkCorrectness:^{
    }];
}

@end

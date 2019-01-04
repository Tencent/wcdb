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

@interface OtherBenchmark : Benchmark

@end

@implementation OtherBenchmark

- (void)test_create_index
{
    int numberOfObjects = 100000;

    WCDB::StatementCreateIndex statement = WCDB::StatementCreateIndex().createIndex(@"testTable_index").table(@"testTable").indexed(BenchmarkObject.identifier);
    __block BOOL result;
    [self
    measure:^{
        result = [self.database execute:statement];
    }
    setUp:^{
        [self.factory setProductionLineObjects:numberOfObjects];
        TestCaseAssertTrue([self.factory production:self.path]);
    }
    tearDown:^{
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

- (void)test_initialization
{
    int numberOfTables = 100000;

    __block BOOL result;
    [self
    measure:^{
        result = [self.database canOpen];
    }
    setUp:^{
        [self.factory setProductionLineTables:numberOfTables];
        TestCaseAssertTrue([self.factory production:self.path]);
    }
    tearDown:^{
        result = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(result);
    }];
}

@end

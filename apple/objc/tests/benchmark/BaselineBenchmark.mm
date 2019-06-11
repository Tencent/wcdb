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

@interface BaselineBenchmark : ObjectsBasedBenchmark

@end

@implementation BaselineBenchmark

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

- (void)test_vacuum
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

@end

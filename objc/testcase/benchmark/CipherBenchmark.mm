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

@interface CipherBenchmark : Benchmark

@end

@implementation CipherBenchmark {
    NSData *_cipher;
}

- (void)setUp
{
    _cipher = [NSData randomData];
}

- (void)test_read
{
    NSString *tableName = [self getTableNameWithIndex:0];
    __block NSArray<BenchmarkObject *> *results = nil;

    [self measure:^{
      results = [self.database getObjectsOfClass:BenchmarkObject.class fromTable:tableName orderBy:BenchmarkObject.key.asOrder(WCTOrderedAscending)];
    }
        setUp:^{
          [self setUpDatabase];
          [self.database setCipherKey:_cipher];
          [self setUpWithPreCreateTable:1];
          [self setUpWithPreCreateObject:self.config.readCount];
          [self setUpWithPreInsertObjects:self.objects intoTable:tableName];

          [self tearDownDatabaseCache];
          [self setUpDatabaseCache];
        }
        tearDown:^{
          [self tearDownDatabase];
        }
        checkCorrectness:^{
          XCTAssertEqual(results.count, self.config.readCount);
          XCTAssertTrue([results isEqualToBenchmarkObjects:self.objects]);
        }];
}

- (void)test_write
{
    NSString *tableName = [self getTableNameWithIndex:0];
    [self measure:^{
      for (BenchmarkObject *object in self.objects) {
          [self.database insertObject:object intoTable:tableName];
      }
    }
        setUp:^{
          [self setUpDatabase];
          [self.database setCipherKey:_cipher];
          [self setUpWithPreCreateTable:1];
          [self setUpWithPreCreateObject:self.config.writeCount];

          [self tearDownDatabaseCache];
          [self setUpDatabaseCache];
        }
        tearDown:^{
          [self tearDownDatabase];
        }
        checkCorrectness:^{
          NSArray<BenchmarkObject *> *objects = [self.database getObjectsOfClass:BenchmarkObject.class fromTable:tableName orderBy:BenchmarkObject.key.asOrder(WCTOrderedAscending)];
          XCTAssertTrue([objects isEqualToBenchmarkObjects:self.objects]);
          XCTAssertEqual(objects.count, self.config.writeCount);
        }];
}

- (void)test_batch_write
{
    NSString *tableName = [self getTableNameWithIndex:0];
    [self measure:^{
      [self.database insertObjects:self.objects intoTable:tableName];
    }
        setUp:^{
          [self setUpDatabase];
          [self.database setCipherKey:_cipher];
          [self setUpWithPreCreateTable:1];
          [self setUpWithPreCreateObject:self.config.batchWriteCount];

          [self tearDownDatabaseCache];
          [self setUpDatabaseCache];
        }
        tearDown:^{
          [self tearDownDatabase];
        }
        checkCorrectness:^{
          NSArray<BenchmarkObject *> *objects = [self.database getObjectsOfClass:BenchmarkObject.class fromTable:tableName orderBy:BenchmarkObject.key.asOrder(WCTOrderedAscending)];
          XCTAssertTrue([objects isEqualToBenchmarkObjects:self.objects]);
          XCTAssertEqual(objects.count, self.config.batchWriteCount);
        }];
}

@end

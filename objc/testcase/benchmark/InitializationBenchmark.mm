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

@interface InitializationBenchmark : Benchmark

@end

@implementation InitializationBenchmark

- (void)test_initialization
{
    __block BOOL result = NO;
    [self measure:^{
      result = self.database.canOpen;
    }
        setUp:^{
          [self setUpDatabase];
          [self setUpWithPreCreateTable:self.config.tableCount];
          [self tearDownDatabaseCache];
        }
        tearDown:^{
          [self tearDownDatabase];
        }
        checkCorrectness:^{
          XCTAssertTrue(result);

          NSArray<WCTValue *> *values = [self.database getColumnOnResult:WCTMaster.name fromTable:WCTMaster.tableName orderBy:WCTMaster.name.asOrder(WCTOrderedAscending)];
          XCTAssertEqual(values.count, self.config.tableCount);

          NSMutableSet<NSString *> *expectedTableNames = [[NSMutableSet<NSString *> alloc] initWithCapacity:values.count];
          for (NSUInteger i = 0; i < values.count; ++i) {
              NSString *expectedTableName = [self getTableNameWithIndex:(int) i];
              [expectedTableNames addObject:expectedTableName];
          }
          XCTAssertNotEqual(expectedTableNames.count, 0);
          for (NSUInteger i = 0; i < values.count; ++i) {
              if ([expectedTableNames containsObject:values[i].stringValue]) {
                  [expectedTableNames removeObject:values[i].stringValue];
              }
          }
          XCTAssertEqual(expectedTableNames.count, 0);
        }];
}

@end

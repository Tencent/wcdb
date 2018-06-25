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

@implementation Benchmark {
    NSMutableArray<BenchmarkObject *> *_objects;
}

- (void)setUp
{
    [super setUp];

    _config = [[Config alloc] init];
    _config.valueLength = 100;
    //    _config.batchWriteCount = 1000000;
    //    _config.readCount = 1000000;
    //    _config.writeCount = 100000;
    //    _config.tableCount = 50000;
    _config.batchWriteCount = 1000;
    _config.readCount = 100;
    _config.writeCount = 100;
    _config.tableCount = 50;
    _config.databaseSize = 500 * 1024 * 1024; //500MB
}

- (void)tearDown
{
    _objects = nil;
    _config = nil;
}

- (void)tearDownDatabase
{
    XCTAssertTrue([self.database removeFiles]);
    [self.database finalizeDatabase];
    _database = nil;
}

- (NSString *)getTableNameWithIndex:(int)index
{
    return [NSString stringWithFormat:@"%@%d", BenchmarkObject.name, index];
}

- (void)setUpWithPreCreateTable:(int)count
{
    XCTAssertTrue([self.database runTransaction:^BOOL(WCTHandle *_Nonnull handle) {
      for (int i = 0; i < count; ++i) {
          NSString *tableName = [self getTableNameWithIndex:i];
          if (![self.database createTableAndIndexes:tableName withClass:BenchmarkObject.class]) {
              return NO;
          }
      }
      return YES;
    }]);
}

- (void)setUpDatabase
{
    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
}

- (void)setUpWithPreInsertObjects:(NSArray<BenchmarkObject *> *)objects intoTable:(NSString *)tableName
{
    XCTAssertTrue([self.database insertObjects:objects intoTable:tableName]);
}

- (void)setUpWithPreCreateObject:(int)count
{
    _objects = [[NSMutableArray<BenchmarkObject *> alloc] init];
    for (int i = 0; i < count; ++i) {
        NSData *data = [NSData randomDataWithLength:self.config.valueLength];
        BenchmarkObject *object = [[BenchmarkObject alloc] initWithKey:i andValue:data];
        [_objects addObject:object];
    }
}

- (void)setUpDatabaseCache
{
    XCTAssertTrue(self.database.canOpen);
}

- (void)tearDownDatabaseCache
{
    [self.database close];
}

- (void)measure:(void (^)(void))block
               setUp:(void (^)(void))setUpBlock
            tearDown:(void (^)(void))tearDownBlock
    checkCorrectness:(void (^)(void))correctnessBlock
{
    [self measureMetrics:self.class.defaultPerformanceMetrics
        automaticallyStartMeasuring:false
                           forBlock:^{

                             tearDownBlock();

                             setUpBlock();

                             [self startMeasuring];

                             block();

                             [self stopMeasuring];

                             correctnessBlock();

                             tearDownBlock();
                           }];
}

@end

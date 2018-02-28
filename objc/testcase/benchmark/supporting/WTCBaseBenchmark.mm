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

#import "WTCBaseBenchmark.h"

@implementation WTCBaseBenchmark {
    NSMutableArray<WTCBenchmarkObject *> *_objects;
}

- (void)setUp
{
    [super setUp];

    WTCBenchmarkObject *dummy = [[WTCBenchmarkObject alloc] init];
    XCTAssertTrue(sizeof(dummy.key) == 8);

    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
    _objects = [[NSMutableArray<WTCBenchmarkObject *> alloc] init];
    _config = WTCConfig.defaultConfig;
    _randomGenerator = [[WTCRandomData alloc] initWithSeed:self.config.randomSeed];
}

- (void)tearDown
{
    _database = nil;
    _objects = nil;
    _config = nil;
    _randomGenerator = nil;
}

- (NSString *)getTableName
{
    return [self getTableNameWithIndex:0];
}

- (NSString *)getTableNameWithIndex:(const int &)index
{
    return [NSString stringWithFormat:@"%@%d", WTCBenchmarkObject.name, index];
}

- (void)setUpWithPreCreateTable
{
    [self setUpWithPreCreateTable:1];
}

- (void)setUpWithPreCreateTable:(const int &)count
{
    XCTAssertTrue([self.database runTransaction:^void {
      for (int i = 0; i < count; ++i) {
          NSString *tableName = [self getTableNameWithIndex:i];
          XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCBenchmarkObject.class]);
      }
    }]);
}

- (void)setUpWithPreInsertObjects:(const int &)count
{
    [self setUpWithPreInsertObjects:count intoIndexedTable:0];
}

- (void)setUpWithPreInsertObjects:(const int &)count intoIndexedTable:(const int &)index
{
    NSMutableArray<WTCBenchmarkObject *> *objects = [[NSMutableArray<WTCBenchmarkObject *> alloc] init];
    for (int i = 0; i < count; ++i) {
        [objects addObject:[[WTCBenchmarkObject alloc] initWithKey:i andValue:[self.randomGenerator dataWithLength:self.config.valueLength]]];
    }
    NSString *tableName = [self getTableNameWithIndex:index];
    XCTAssertTrue([self.database insertObjects:objects into:tableName]);
}

- (void)setUpWithPreCreateObject:(const int &)count
{
    for (int i = 0; i < count; ++i) {
        [_objects addObject:[[WTCBenchmarkObject alloc] initWithKey:i andValue:[self.randomGenerator dataWithLength:self.config.valueLength]]];
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

- (void)tearDownDatabase
{
    [self.database close:^{
      XCTAssertTrue([self.database removeFilesWithError:nil]);
    }];
}

- (void)mesasure:(void (^)(void))setUpBlock
             for:(void (^)(void))block 
checkCorrectness:(void (^)(void))correctnessBlock
{
    [self measureMetrics:self.class.defaultPerformanceMetrics
        automaticallyStartMeasuring:false
                           forBlock:^{
                             setUpBlock();

                             [self startMeasuring];

                             block();

                             [self stopMeasuring];

                             correctnessBlock();
                           }];
}

@end

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

#import "Assert.h"
#import "BenchmarkObject+WCTTableCoding.h"
#import "BenchmarkObject.h"
#import "Config.h"
#import "Convenience.h"
#import "TestCase.h"

@interface Benchmark : TestCase

@property (readonly) Config *config;

@property (retain) WCTDatabase *database;

@property (readonly) WCTDatabase *recommendedDatabase;

@property (readonly) WCTDatabase *cachedDatabase;

@property (readonly) NSArray<BenchmarkObject *> *objects;

- (NSString *)getTableNameWithIndex:(int)index;

- (void)setUpDatabase;

- (void)tearDownDatabase;

- (void)setUpWithPreCreateTable:(int)count;

- (void)setUpWithPreInsertObjects:(NSArray<BenchmarkObject *> *)objects intoTable:(NSString *)tableName;

- (void)setUpWithPreCreateObject:(int)count;

- (void)setUpDatabaseCache;

- (void)tearDownDatabaseCache;

- (void)measure:(void (^)(void))block
           setUp:(void (^)(void))setUpBlock
        tearDown:(void (^)(void))tearDownBlock
checkCorrectness:(void (^)(void))correctnessBlock;

@end

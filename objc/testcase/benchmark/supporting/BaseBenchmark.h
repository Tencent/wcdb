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
#import "RandomData.h"
#import "WCTTestCase.h"

@interface BaseBenchmark : WCTTestCase

@property(readonly) Config *config;

@property(readonly) WCTDatabase *database;

@property(readonly) RandomData *randomGenerator;

@property(readonly) NSArray<BenchmarkObject *> *objects;

- (NSString *)getTableName;
- (NSString *)getTableNameWithIndex:(const int &)index;

- (void)setUpWithPreCreateTable;
- (void)setUpWithPreCreateTable:(const int &)count;

- (void)setUpWithPreInsertObjects:(const int &)count;
- (void)setUpWithPreInsertObjects:(const int &)count intoIndexedTable:(const int &)index;

- (void)setUpWithPreCreateObject:(const int &)count;

- (void)setUpDatabaseCache;

- (void)tearDownDatabaseCache;

- (void)tearDownDatabase;

- (void)mesasure:(void (^)(void))setUpBlock
             for:(void (^)(void))block 
checkCorrectness:(void (^)(void))correctnessBlock;

@end

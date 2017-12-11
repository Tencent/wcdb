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

#import "FBMBaselineWrite.h"

@implementation FBMBaselineWrite {
    FMDatabase *_database;
    NSArray *_objects;
}

+ (NSString *)benchmarkType
{
    return [WCTBenchmarkTypeBaselineWrite copy];
}

- (void)prepare
{
    FMDatabase *database = [[FMDatabase alloc] initWithPath:_path];
    [self openDatabase:database];
    if (![database beginTransaction]) {
        abort();
    }
    NSString *sql = [NSString stringWithFormat:@"CREATE TABLE %@(key INTEGER,value BLOB);", _tableName];
    if (![database executeUpdate:sql]) {
        abort();
    }
    if (![database commit]) {
        abort();
    }
    if (![database close]) {
        abort();
    }
}

- (void)preBenchmark
{
    _database = [[FMDatabase alloc] initWithPath:_path];
    [self openDatabase:_database];

    NSMutableArray *objects = [[NSMutableArray alloc] init];
    for (int i = 0; i < _config.writeCount; ++i) {
        FBMObject *object = [[FBMObject alloc] init];
        object.key = i;
        object.value = [_randomGenerator dataWithLength:_config.valueLength];
        [objects addObject:object];
    }
    _objects = objects;
}

- (NSUInteger)benchmark
{
    NSString *sql = [NSString stringWithFormat:@"INSERT INTO %@ VALUES(?, ?)", _tableName];
    for (FBMObject *object in _objects) {
        if (![_database executeUpdate:sql, @(object.key), object.value]) {
            abort();
        }
    }
    return _objects.count;
}

- (void)postBenchmark
{
    [_database close];
}

@end

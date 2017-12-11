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

#import "FBMBaselineRead.h"

@implementation FBMBaselineRead {
    FMDatabase *_database;
    NSMutableArray *_result;
}

+ (NSString *)benchmarkType
{
    return [WCTBenchmarkTypeBaselineRead copy];
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
    sql = [NSString stringWithFormat:@"INSERT INTO %@ VALUES(?, ?)", _tableName];
    for (int i = 0; i < _config.readCount; ++i) {
        NSData *d = [_randomGenerator dataWithLength:_config.valueLength];
        if (![database executeUpdate:sql, @(i), d]) {
            abort();
        }
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
}

- (NSUInteger)benchmark
{
    _result = [[NSMutableArray alloc] init];
    NSString *sql = [NSString stringWithFormat:@"SELECT * FROM %@", _tableName];
    FMResultSet *resultSet = [_database executeQuery:sql];
    while ([resultSet next]) {
        FBMObject *object = [[FBMObject alloc] init];
        object.key = [resultSet intForColumnIndex:0];
        object.value = [resultSet dataForColumnIndex:1];
        [_result addObject:object];
    }
    return _result.count;
}

- (void)postBenchmark
{
    [_database close];
}

@end

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

#import "FBMMultithreadReadWrite.h"

@implementation FBMMultithreadReadWrite {
    FMDatabasePool *_databasePool;
    dispatch_group_t _group;
    dispatch_queue_t _queue;
    NSArray *_objects;
    NSMutableArray *_result;
}

+ (NSString *)benchmarkType
{
    return [WCTBenchmarkTypeMultithreadReadWrite copy];
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
    _databasePool = [[FMDatabasePool alloc] initWithPath:_path];
    _databasePool.delegate = self;

    _group = dispatch_group_create();
    _queue = dispatch_queue_create(self.class.name.UTF8String, DISPATCH_QUEUE_CONCURRENT);
    NSMutableArray *objects = [[NSMutableArray alloc] init];
    for (int i = 0; i < _config.batchWriteCount; ++i) {
        FBMObject *object = [[FBMObject alloc] init];
        object.key = i + (int) _config.readCount;
        object.value = [_randomGenerator dataWithLength:_config.valueLength];
        [objects addObject:object];
    }
    _objects = objects;
}

- (NSUInteger)benchmark
{
    NSString *sql = [NSString stringWithFormat:@"SELECT * FROM %@", _tableName];
    dispatch_group_async(_group, _queue, ^{
      [_databasePool inDatabase:^(FMDatabase *_Nonnull db) {
        NSMutableArray *objects = [[NSMutableArray alloc] init];
        FMResultSet *resultSet = [db executeQuery:sql];
        while ([resultSet next]) {
            FBMObject *object = [[FBMObject alloc] init];
            object.key = [resultSet intForColumnIndex:0];
            object.value = [resultSet dataForColumnIndex:1];
            [objects addObject:object];
        }
        _result = objects;
        if (_result.count != _config.readCount) {
            abort();
        }
      }];
    });
    dispatch_group_async(_group, _queue, ^{
      [_databasePool inDatabase:^(FMDatabase *_Nonnull db) {
        NSString *sql = [NSString stringWithFormat:@"INSERT INTO %@ VALUES(?, ?)", _tableName];
        if (![db beginTransaction]) {
            abort();
        }
        for (FBMObject *object in _objects) {
            if (![db executeUpdate:sql, @(object.key), object.value]) {
                abort();
            }
        }
        if (![db commit]) {
            abort();
        }
      }];
    });
    dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    return _result.count + _objects.count;
}

- (BOOL)databasePool:(FMDatabasePool *)pool shouldAddDatabaseToPool:(FMDatabase *)database
{
    [self openDatabase:database];
    return YES;
}

@end

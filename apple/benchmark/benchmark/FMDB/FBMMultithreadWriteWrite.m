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

#import "FBMMultithreadWriteWrite.h"

@implementation FBMMultithreadWriteWrite {
    FMDatabasePool *_databasePool;
    dispatch_group_t _group;
    dispatch_queue_t _queue;
    NSArray *_objects1;
    NSArray *_objects2;
}

+ (NSString *)benchmarkType
{
    return nil;
}

- (void)prepare
{
    FMDatabase *database = [[FMDatabase alloc] initWithPath:_path];
    [self openDatabase:database];
    NSString *sql = [NSString stringWithFormat:@"CREATE TABLE %@(key INTEGER,value BLOB);", _tableName];
    if (![database executeUpdate:sql]) {
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
    NSMutableArray *objects1 = [[NSMutableArray alloc] init];
    NSMutableArray *objects2 = [[NSMutableArray alloc] init];
    for (int i = 0; i < _config.writeCount; ++i) {
        {
            FBMObject *object1 = [[FBMObject alloc] init];
            object1.key = i;
            object1.value = [_randomGenerator dataWithLength:_config.valueLength];
            [objects1 addObject:object1];
        }
        {
            FBMObject *object2 = [[FBMObject alloc] init];
            object2.key = i + (int) _config.writeCount;
            object2.value = [_randomGenerator dataWithLength:_config.valueLength];
            [objects2 addObject:object2];
        }
    }
    _objects1 = objects1;
    _objects2 = objects2;
}

- (NSUInteger)benchmark
{
    // not supported
    //    dispatch_group_async(_group, _queue, ^{
    //      [_databasePool inDatabase:^(FMDatabase *_Nonnull db) {
    //        [db setShouldCacheStatements:YES];
    //        NSString *sql = [NSString stringWithFormat:@"INSERT INTO %@ VALUES(?, ?)", _tableName];
    //        for (FBMObject *object in _objects1) {
    //            if (![db executeUpdate:sql, @(object.key), object.value]) {
    //                abort();
    //            }
    //        }
    //      }];
    //    });
    //    dispatch_group_async(_group, _queue, ^{
    //      [_databasePool inDatabase:^(FMDatabase *_Nonnull db) {
    //        [db setShouldCacheStatements:YES];
    //        NSString *sql = [NSString stringWithFormat:@"INSERT INTO %@ VALUES(?, ?)", _tableName];
    //        for (FBMObject *object in _objects2) {
    //            if (![db executeUpdate:sql, @(object.key), object.value]) {
    //                abort();
    //            }
    //        }
    //      }];
    //    });
    //    dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    return _objects1.count + _objects2.count;
}

- (BOOL)databasePool:(FMDatabasePool *)pool shouldAddDatabaseToPool:(FMDatabase *)database
{
    [self openDatabase:database];
    return YES;
}

@end

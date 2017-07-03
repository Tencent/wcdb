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

#import "WBMMultithreadWriteWrite.h"

@implementation WBMMultithreadWriteWrite {
    WCTDatabase *_database;
    dispatch_group_t _group;
    dispatch_queue_t _queue;
    NSArray *_objects1;
    NSArray *_objects2;
}

+ (const NSString *)benchmarkType
{
    return WCTBenchmarkTypeMultithreadWriteWrite;
}

- (void)prepare
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:_path];
    {
        BOOL result = [database createTableAndIndexesOfName:_tableName withClass:WBMObject.class];
        if (!result) {
            abort();
        }
    }
    [database close];
}

- (void)preBenchmark
{
    _database = [[WCTDatabase alloc] initWithPath:_path];

    _group = dispatch_group_create();
    _queue = dispatch_queue_create(self.class.name.UTF8String, DISPATCH_QUEUE_CONCURRENT);
    NSMutableArray *objects1 = [[NSMutableArray alloc] init];
    NSMutableArray *objects2 = [[NSMutableArray alloc] init];
    for (int i = 0; i < _config.batchWriteCount; ++i) {
        {
            WBMObject *object = [[WBMObject alloc] init];
            object.key = i;
            object.value = [_randomGenerator dataWithLength:_config.valueLength];
            [objects1 addObject:object];
        }
        {
            WBMObject *object = [[WBMObject alloc] init];
            object.key = i + (int) _config.writeCount;
            object.value = [_randomGenerator dataWithLength:_config.valueLength];
            [objects2 addObject:object];
        }
    }
    _objects1 = objects1;
    _objects2 = objects2;
}

- (NSUInteger)benchmark
{
    dispatch_group_async(_group, _queue, ^{
      BOOL result = [_database insertObjects:_objects1 into:_tableName];
      if (!result) {
          abort();
      }
    });
    dispatch_group_async(_group, _queue, ^{
      BOOL result = [_database insertObjects:_objects2 into:_tableName];
      if (!result) {
          abort();
      }
    });
    dispatch_group_wait(_group, DISPATCH_TIME_FOREVER);
    return _objects1.count + _objects2.count;
}

@end

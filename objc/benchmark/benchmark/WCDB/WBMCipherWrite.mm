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

#import "WBMCipherWrite.h"

@implementation WBMCipherWrite {
    WCTDatabase *_database;
    NSArray *_objects;
}

+ (const NSString *)benchmarkType
{
    return WCTBenchmarkTypeCipherWrite;
}

- (void)prepare
{
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:_path];
    [database setCipherKey:[@"benchmark" dataUsingEncoding:NSASCIIStringEncoding]];
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
    [_database setCipherKey:[@"benchmark" dataUsingEncoding:NSASCIIStringEncoding]];
    if (![_database canOpen]) {
        abort();
    }
    NSMutableArray *objects = [[NSMutableArray alloc] init];
    for (int i = 0; i < _config.batchWriteCount; ++i) {
        WBMObject *object = [[WBMObject alloc] init];
        object.key = i;
        object.value = [_randomGenerator dataWithLength:_config.valueLength];
        [objects addObject:object];
    }
    _objects = objects;
}

- (NSUInteger)benchmark
{
    BOOL result = [_database insertObjects:_objects into:_tableName];
    if (!result) {
        abort();
    }
    return _objects.count;
}

@end

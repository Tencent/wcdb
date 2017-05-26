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

#import "benchmark_normal.h"
#import "WCTBenchmarkStaticstic.h"
#import "WCTBenchmarkObject.h"

void benchmark_normal(NSString* baseDirectory)
{
    NSString* className = NSStringFromClass(WCTBenchmarkObject.class);
    NSString* path = [baseDirectory stringByAppendingPathComponent:className];
    NSString* tableName = className; 
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
    [database close:^{
        [database removeFilesWithError:nil];
    }];

    {
        BOOL ret = [database createTableAndIndexesOfName:tableName 
                                               withClass:WCTBenchmarkObject.class];
        assert(ret);
    }
    
    int count = 100000;
    {
        NSMutableArray* objects = [[NSMutableArray alloc] init];
        for (int i = 0; i < count; ++i) {
            WCTBenchmarkObject* object = [[WCTBenchmarkObject alloc] init];
            object.intValue = i;
            object.stringValue = [NSString stringWithFormat:@"%d", i];
            [objects addObject:object];
        }
        WCTTable* table = [database getTableOfName:tableName
                                         withClass:WCTBenchmarkObject.class];
        WCTBenchmarkStaticstic* staticstic = [[WCTBenchmarkStaticstic alloc] init];
        [staticstic start];
        BOOL ret = [table insertObjects:objects];
        [staticstic stop];
        assert(ret);
        NSLog(@"Insert %d objects cost %f ms", count, staticstic.getElapseTimes[0].doubleValue);
    }
    
    {
        WCTTable* table = [database getTableOfName:tableName
                                         withClass:WCTBenchmarkObject.class];
        WCTBenchmarkStaticstic* staticstic = [[WCTBenchmarkStaticstic alloc] init];
        [staticstic start];
        NSArray* objects = [table getAllObjects];
        [staticstic stop];
        NSLog(@"Select %lu objects cost %f ms", objects.count, staticstic.getElapseTimes[0].doubleValue);
    }
}

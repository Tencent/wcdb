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

#import "sample_statistics_main.h"
#import "WCTSampleStatistics+WCTTableCoding.h"
#import "WCTSampleStatistics.h"

void sample_statistics_main(NSString *baseDirectory)
{
    NSLog(@"Sample-statistics Begin");
    NSString *className = NSStringFromClass(WCTSampleStatistics.class);
    NSString *path = [baseDirectory stringByAppendingPathComponent:className];
    NSString *tableName = className;
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    [database close:^{
      [database removeFilesWithError:nil];
    }];

    //trace
    //You should register trace before all db operation.
    [WCTStatistics SetGlobalPerformanceTrace:^(WCTTag tag, NSDictionary<NSString *, NSNumber *> *sqls, NSInteger cost) {
      NSLog(@"Tag: %d", tag);
      [sqls enumerateKeysAndObjectsUsingBlock:^(NSString *sql, NSNumber *count, BOOL *) {
        NSLog(@"SQL: %@ Count: %d", sql, count.intValue);
      }];
      NSLog(@"Total cost %ld nanoseconds", (long) cost);
    }];

    //SQL
    [WCTStatistics SetGlobalSQLTrace:^(NSString *sql) {
      NSLog(@"SQL: %@", sql);
    }];

    //error
    [WCTStatistics SetGlobalErrorReport:^(WCTError *error) {
      NSLog(@"[Error] %@", error);
    }];

    [database createTableAndIndexesOfName:tableName withClass:WCTSampleStatistics.class];

    //trace for insertion
    {
        WCTSampleStatistics *object = [[WCTSampleStatistics alloc] init];
        object.intValue = 100;
        [database insertObject:object into:tableName];
    }

    //trace for transaction
    {
        NSMutableArray *objects = [[NSMutableArray alloc] init];
        for (int i = 0; i < 10; ++i) {
            WCTSampleStatistics *object = [[WCTSampleStatistics alloc] init];
            object.intValue = i;
        }
        [database insertObjects:objects into:tableName];

        [database runTransaction:^BOOL {
          NSArray *results = [database getAllObjectsOfClass:WCTSampleStatistics.class
                                                  fromTable:tableName];
          for (WCTSampleStatistics *object in results) {
              object.intValue = -object.intValue;
          }
          [database insertObjects:results into:tableName];
          return YES;
        }];
    }

    //error
    {
        WCTSampleStatistics *object = [[WCTSampleStatistics alloc] init];
        object.intValue = 100;
        [database insertObject:object into:tableName];
    }

    //unregister
    [WCTStatistics SetGlobalPerformanceTrace:nil];
    [WCTStatistics SetGlobalErrorReport:nil];
    [WCTStatistics SetGlobalSQLTrace:nil];
}

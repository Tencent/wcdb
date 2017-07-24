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

#import <Foundation/Foundation.h>
#import <WCDB/WCTDeclare.h>

/**
 Trigger when error occurs
 */
typedef void (^WCTErrorReport)(WCTError *);

/**
 Trigger when a transaction or a normal sql ends.
 */
typedef void (^WCTPerformanceTrace)(WCTTag, NSDictionary<NSString *, NSNumber *> *, NSInteger);

/**
 Trigger when a SQL is executed.
 */
typedef void (^WCTSQLTrace)(NSString *);

/**
 Statistics
 */
@interface WCTStatistics : NSObject

/**
 @brief You can register a reporter to monitor all errors.
 
     [WCTStatistics SetGlobalErrorReport:^(WCTError* error) {
        NSLog(@"%@", error);
     }];
 
 @param report report
 @see WCTErrorReport
 */
+ (void)SetGlobalErrorReport:(WCTErrorReport)report;

/**
 @brief You can register a tracer to monitor the performance of all SQLs.
        It returns 
        1. The collection of SQLs and the executions count of each SQL.
        2. Time consuming in nanoseconds.
        3. Tag of database.
        Note that:
        1. You should register trace before all db operations. 
        2. Global tracer will be recovered by db tracer.
 
    [WCTStatistics SetGlobalPerformanceTrace:^(WCTTag tag, NSDictionary<NSString*, NSNumber*>* sqls, NSInteger cost) {
        NSLog(@"Tag: %d", tag);
        [sqls enumerateKeysAndObjectsUsingBlock:^(NSString *sql, NSNumber *count, BOOL *) {
            NSLog(@"SQL: %@ Count: %d", sql, count.intValue);
        }];
        NSLog(@"Total cost %ld nanoseconds", (long)cost);
    }];
 
 @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
 @param trace trace
 @see WCTPerformanceTrace
 @see [WCTDatabase setPerformanceTrace:]
 */
+ (void)SetGlobalPerformanceTrace:(WCTPerformanceTrace)trace;

/**
 @brief You can register a tracer to monitor the execution of all SQLs.
        It returns a prepared or executed SQL.
        Note that you should register trace before all db operations. 
 
    [WCTStatistics SetGlobalSQLTrace:^(NSString* sql) {
        NSLog(@"SQL: %@", sql);
    }];
 
 @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
 @see WCTSQLTrace
 @param trace trace
 */
+ (void)SetGlobalSQLTrace:(WCTSQLTrace)trace;

@end

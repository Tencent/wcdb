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

typedef void (^WCTErrorReport)(WCTError*);

typedef void (^WCTTrace)(WCTTag, NSDictionary<NSString*, NSNumber*>*, NSInteger);

@interface WCTStatictics : NSObject

/**
 You can register a reporter to monitor all errors.
 
 @param report report
 */
+ (void)SetGlobalErrorReport:(WCTErrorReport)report;

/**
 You can register a tracer to monitor the performance of all SQLs.
 It returns 
 1. The collection of SQLs and the executions count of each SQL.
 2. Time consuming in nanoseconds.
 3. Tag of database.
 
 [WCTDatabase SetGlobalTrace:^(WCTTag tag, NSDictionary<NSString*, NSNumber*>* sqls, NSInteger cost) {
 NSLog(@"Tag: %d", tag);
 [sqls enumerateKeysAndObjectsUsingBlock:^(NSString *sql, NSNumber *count, BOOL *) {
 NSLog(@"SQL: %@ Count: %d", sql, count.intValue);
 }];
 NSLog(@"Total cost %lld nanoseconds", cost);
 }];
 
 Note that tracer may slow down WCDB.
 
 @param trace trace
 */
+ (void)SetGlobalTrace:(WCTTrace)trace;

@end

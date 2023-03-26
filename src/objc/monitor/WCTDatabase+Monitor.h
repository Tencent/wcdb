//
// Created by sanhuazhang on 2019/05/02
//

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

#import "WCTCommon.h"
#import "WCTDatabase.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Trigger when error occurs
 */
typedef void (^WCTErrorTraceBlock)(WCTError*);

/**
 Triggered when a transaction or a normal sql ends.
 */
typedef void (^WCTPerformanceTraceBlock)(NSString* /* path */, uint64_t /*handleIdentifier*/, NSString* /* sql */, double /* cost */);

/**
 Triggered when a SQL is executed.
 */
typedef void (^WCTSQLTraceBlock)(NSString* /* path */, uint64_t /*handleIdentifier*/, NSString* /* sql */);

typedef NS_ENUM(NSUInteger, WCTDatabaseOperation) {
    WCTDatabaseOperation_Create = 0,
    WCTDatabaseOperation_SetTag,
    WCTDatabaseOperation_OpenHandle,
};

/**
 Triggered when a specific event of database occurs.
 */
typedef void (^WCTDatabaseOperationTraceBlock)(WCTDatabase* /* database */, WCTDatabaseOperation /* type of operation*/);

@interface WCTDatabase (Monitor)

/**
 @brief You can register a reporter to monitor all errors.
 
     [WCTDatabase globalTraceError:^(WCTError* error) {
        NSLog(@"%@", error);
     }];
 
 @param block block
 @see `WCTErrorTraceBlock`
 */
+ (void)globalTraceError:(nullable WCDB_ESCAPE WCTErrorTraceBlock)block;

/**
 @brief You can register a reporter to monitor all errors of current database,.
 @param block block
 @see `WCTErrorTraceBlock`
 */
- (void)traceError:(nullable WCDB_ESCAPE WCTErrorTraceBlock)block;

/**
 @brief You can register a tracer to monitor the performance of all SQLs.
 It returns:
     1. Every SQL executed by the database.
     2. Time consuming in seconds.
     3. Path of database.
     4. The id of the handle executing this SQL.
 
 @note  You should register trace before all db operations. Global tracer and db tracer do not interfere with each other.
 
     [WCTDatabase globalTracePerformance:^(NSString* path,  uint64_t handleIdentifier, NSString* sql , double cost) {
        NSLog(@"Path: %%@", path);
        NSLog(@"The handle with id %llu took %f seconds to execute %@",  handleIdentifier, cost, sql);
     }];
 
 @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
 @param trace trace
 @see `WCTPerformanceTraceBlock`
 */
+ (void)globalTracePerformance:(nullable WCDB_ESCAPE WCTPerformanceTraceBlock)trace;

/**
 @brief You can register a tracer to monitor the performance of all SQLs executed in the current database.
 @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
 @param trace trace
 @see `WCTPerformanceTraceBlock`
 */
- (void)tracePerformance:(nullable WCDB_ESCAPE WCTPerformanceTraceBlock)trace;

/**
 @brief You can register a tracer to monitor the execution of all SQLs.
 It returns:
     1. Every SQL executed by the database.
     2. Path of database.
     3. The id of the handle executing this SQL.
 
 @note  You should register trace before all db operations. Global tracer and db tracer do not interfere with each other.
 
     [WCTDatabase globalTraceSQL:^(NSString* path,  uint64_t handleIdentifier, NSString* sql , double cost) {
        NSLog(@"Path: %%@", path);
        NSLog(@"The handle with id %llu executed %@",  handleIdentifier, sql);
     }];
 
 @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
 @see `WCTSQLTraceBlock`
 @param trace trace
 */
+ (void)globalTraceSQL:(nullable WCDB_ESCAPE WCTSQLTraceBlock)trace;

/**
 @brief You can register a tracer to monitor the execution of all SQLs executed in the current database.
 @note  You should register trace before all db operations.
 @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
 @see `WCTSQLTraceBlock`
 @param trace trace
 */
- (void)traceSQL:(nullable WCDB_ESCAPE WCTSQLTraceBlock)trace;

/**
 @brief You can register a tracer to these database events:
     1. creating a database object for the first time;
     2. setting a tag on the database;
     3. opening a new database handle.
 @see `WCTDatabaseOperationTraceBlock`
 @param trace tracer.
 */
+ (void)globalTraceDatabaseOperation:(nullable WCDB_ESCAPE WCTDatabaseOperationTraceBlock)trace;

@end

NS_ASSUME_NONNULL_END

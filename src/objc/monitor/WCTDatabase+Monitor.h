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
#import "WCTPerformanceInfo.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Trigger when error occurs
 */
typedef void (^WCTErrorTraceBlock)(WCTError*);

/**
 Triggered when a transaction or a normal sql ends.
 */
typedef void (^WCTPerformanceTraceBlock)(WCTTag /* tag */, NSString* /* path */, uint64_t /*handleIdentifier*/, NSString* /* sql */, WCTPerformanceInfo* /* info */);

/**
 Triggered when a SQL is executed.
 */
typedef void (^WCTSQLTraceBlock)(WCTTag /* tag */, NSString* /* path */, uint64_t /*handleIdentifier*/, NSString* /* sql */, NSString* /*info*/);

typedef NS_ENUM(NSUInteger, WCTDatabaseOperation) {
    WCTDatabaseOperation_Create = 0,
    WCTDatabaseOperation_SetTag,
    WCTDatabaseOperation_OpenHandle,
};

/**
 Triggered when a specific event of database occurs.
 */
typedef void (^WCTDatabaseOperationTraceBlock)(WCTDatabase* /* database */, WCTDatabaseOperation /* type of operation*/, NSDictionary* /* infos about current operation */);

/**
 Triggered when the database operation is blocked by other threads.
 */
typedef void (^WCTDatabaseBusyTraceBlock)(WCTTag /* tag */,
                                          NSString* /* path */,
                                          uint64_t /* id of the thread being waited on */,
                                          NSString* /* sql executing in the thread being waited on */
);

/**
 The following are the keys in the infos from the callback of database operation monitoring.
 */
// The number of alive handles to the current database
WCDB_EXTERN NSString* const WCTDatabaseMonitorInfoKeyHandleCount;
// The time in microseconds spent to open and config the current handle.
WCDB_EXTERN NSString* const WCTDatabaseMonitorInfoKeyHandleOpenTime;
// The cpu time in microseconds spent to open and config the current handle.
WCDB_EXTERN NSString* const WCTDatabaseMonitorInfoKeyHandleOpenCPUTime;
// The memory in bytes used to store the schema in sqlite handle.
WCDB_EXTERN NSString* const WCTDatabaseMonitorInfoKeySchemaUsage;
// The number of tables in the current database.
WCDB_EXTERN NSString* const WCTDatabaseMonitorInfoKeyTableCount;
// The number of indexes in the current database.
WCDB_EXTERN NSString* const WCTDatabaseMonitorInfoKeyIndexCount;
// The number of triggers in the current database.
WCDB_EXTERN NSString* const WCTDatabaseMonitorInfoKeyTriggerCount;

WCDB_API @interface WCTDatabase(Monitor)

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
     2. Time consuming in nanoseconds.
     3. Number of reads and writes on different types of db pages.
     4. Tag of database.
     5. Path of database.
     6. The id of the handle executing this SQL.
 
 @note  You should register trace before all db operations. Global tracer and db tracer do not interfere with each other.
 
     [WCTDatabase globalTracePerformance:^(WCTTag tag, NSString* path, uint64_t handleIdentifier, NSString* sql, WCTPerformanceInfo *info) {
        NSLog(@"Tag: %ld", tag);
        NSLog(@"Path: %@", path);
        NSLog(@"The handle with id %llu took %lld nanoseconds to execute %@", handleIdentifier, info.costInNanoseconds, sql);
     }];
 
 @warning Tracer may cause WCDB performance degradation, according to your needs to choose whether to open.
 @param trace trace
 @see `WCTPerformanceTraceBlock`
 */
+ (void)globalTracePerformance:(nullable WCDB_ESCAPE WCTPerformanceTraceBlock)trace;

/**
 @brief You can register a tracer to monitor the performance of all SQLs executed in the current database.
 @warning Tracer may cause WCDB performance degradation, according to your needs to choose whether to open.
 @param trace trace
 @see `WCTPerformanceTraceBlock`
 */
- (void)tracePerformance:(nullable WCDB_ESCAPE WCTPerformanceTraceBlock)trace;

/**
 @brief You can register a tracer to monitor the execution of all SQLs.
 It returns:
     1. Every SQL executed by the database.
     2. Tag of database.
     3. Path of database.
     4. The id of the handle executing this SQL.
     5. Detailed execution information of SQL. It is valid only when full sql trace is enable.
 
 @note  You should register trace before all db operations. Global tracer and db tracer do not interfere with each other.
 
     [WCTDatabase globalTraceSQL:^(WCTTag tag, NSString* path, uint64_t handleIdentifier, NSString* sql, NSString *info) {
        NSLog(@"Tag: %ld", tag);
        NSLog(@"Path: %%@", path);
        NSLog(@"The handle with id %llu executed %@", handleIdentifier, sql);
        NSLog(@"Excution info %@", info);
     }];
 
 @warning Tracer may cause WCDB performance degradation, according to your needs to choose whether to open.
 @see `WCTSQLTraceBlock`
 @param trace trace
 */
+ (void)globalTraceSQL:(nullable WCDB_ESCAPE WCTSQLTraceBlock)trace;

/**
 @brief You can register a tracer to monitor the execution of all SQLs executed in the current database.
 @note  You should register trace before all db operations.
 @warning Tracer may cause WCDB performance degradation, according to your needs to choose whether to open.
 @see `WCTSQLTraceBlock`
 @param trace trace
 */
- (void)traceSQL:(nullable WCDB_ESCAPE WCTSQLTraceBlock)trace;

/**
 @brief Enable to collect more SQL execution information in SQL tracer.
 @note  The detailed execution information of sql will include all bind parameters, step counts of `SELECT` statement,
    last inserted rowid of `INSERT` statement, changes of `UPDATE` and `DELETE` statements.
    These infomation will be returned in the last parameter of `WCTSQLTraceBlock`.
 @warning Collecting these infomation will significantly reduce the performance of WCDB,
    please enable it only when necessary, and disable it when unnecessary.
 @see `WCTSQLTraceBlock`
 @param enable enable or not.
 */
- (void)enableFullSQLTrace:(BOOL)enable;

/**
 @brief You can register a tracer to these database events:
     1. creating a database object for the first time;
     2. setting a tag on the database;
     3. opening a new database handle.
 @see `WCTDatabaseOperationTraceBlock`
 @param trace tracer.
 */
+ (void)globalTraceDatabaseOperation:(nullable WCDB_ESCAPE WCTDatabaseOperationTraceBlock)trace;

/**
 @brief You can register a tracer to database busy events.
 It returns:
     1. Tag of database being busy.
     2. Path of database being busy.
     3. ID of the thread being waited on.
     4. SQL executing in the thread being waited on.
 @warning Since the tracer will be called back synchronously
    when the database operation is blocked and times out,
    you can neither directly access the busy database
    nor perform heavy operation in the tracer.
 @see `WCTDatabaseBusyTraceBlock`
 @param trace tracer.
 @param timeOut timeout in seconds for blocking database operation
 */
+ (void)globalTraceBusy:(nullable WCDB_ESCAPE WCTDatabaseBusyTraceBlock)trace withTimeOut:(double)timeOut;

@end

NS_ASSUME_NONNULL_END

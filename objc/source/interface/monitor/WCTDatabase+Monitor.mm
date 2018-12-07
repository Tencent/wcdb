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

#import <WCDB/Assertion.hpp>
#import <WCDB/Console.hpp>
#import <WCDB/Notifier.hpp>
#import <WCDB/WCTDatabase+Monitor.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTPerformanceFootprint.h>

@implementation WCTDatabase (Monitor)

+ (BOOL)debuggable
{
    return WCDB::Console::debuggable();
}

+ (void)setDebuggable:(BOOL)debuggable
{
    WCDB::Console::shared()->setDebuggable(debuggable);
}

+ (void)globalTraceError:(WCTErrorTraceBlock)block
{
    WCDB::Notifier::Callback callback = nullptr;
    if (block) {
        callback = [block](const WCDB::Error &error) {
            WCTError *nsError = [[WCTError alloc] initWithError:error];
            block(nsError);
        };
    }
    WCDB::Console::shared()->setLogger(callback);
}

+ (void)resetGlobalErrorTracer
{
    WCDB::Console::shared()->setLogger(WCDB::Console::log);
}

+ (void)globalTracePerformance:(WCTPerformanceTraceBlock)trace
{
    WCDB::Handle::PerformanceNotification callback = nullptr;
    if (trace) {
        callback = [trace](const WCDB::HandleNotification::Footprints &footprints, const int64_t &cost) {
            NSMutableArray<WCTPerformanceFootprint *> *array = [[NSMutableArray<WCTPerformanceFootprint *> alloc] init];
            for (const auto &footprint : footprints) {
                NSString *sql = [NSString stringWithUTF8String:footprint.sql.c_str()];
                [array addObject:[[WCTPerformanceFootprint alloc] initWithSQL:sql andFrequency:footprint.frequency]];
            }
            trace(array, (NSUInteger) cost);
        };
    }
    WCDB::Core::shared()->setNotificationForGlobalPerformanceTrace(callback);
}

+ (void)globalTraceSQL:(WCTSQLTraceBlock)trace
{
    WCDB::Handle::SQLNotification callback = nullptr;
    if (trace) {
        callback = [trace](const WCDB::String &sql) {
            trace([NSString stringWithUTF8String:sql.c_str()]);
        };
    }
    WCDB::Core::shared()->setNotificationForGlobalSQLTrace(callback);
}

- (void)tracePerformance:(WCTPerformanceTraceBlock)trace
{
    WCTDatabaseAssert(return;);
    WCDB::Handle::PerformanceNotification callback = nullptr;
    if (trace) {
        callback = [trace](const WCDB::HandleNotification::Footprints &footprints, const int64_t &cost) {
            NSMutableArray<WCTPerformanceFootprint *> *array = [[NSMutableArray<WCTPerformanceFootprint *> alloc] init];
            for (const auto &footprint : footprints) {
                NSString *sql = [NSString stringWithUTF8String:footprint.sql.c_str()];
                [array addObject:[[WCTPerformanceFootprint alloc] initWithSQL:sql andFrequency:footprint.frequency]];
            }
            trace(array, (NSUInteger) cost);
        };
        _database->setConfig(WCDB::Core::performanceTraceConfigName,
                             WCDB::Core::shared()->performanceTraceConfig(callback),
                             WCDB::Configs::Priority::Highest);
    } else {
        _database->removeConfig(WCDB::Core::performanceTraceConfigName);
    }
}

- (void)traceSQL:(WCTSQLTraceBlock)trace
{
    WCTDatabaseAssert(return;);
    WCDB::Handle::SQLNotification callback = nullptr;
    if (trace) {
        callback = [trace](const WCDB::String &sql) {
            trace([NSString stringWithUTF8String:sql.c_str()]);
        };
        _database->setConfig(WCDB::Core::sqlTraceConfigName,
                             WCDB::Core::shared()->sqlTraceConfig(callback),
                             WCDB::Configs::Priority::Highest);
    } else {
        _database->removeConfig(WCDB::Core::sqlTraceConfigName);
    }
}

@end

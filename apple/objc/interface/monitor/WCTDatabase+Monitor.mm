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

namespace WCDB {

void Console::print(const String &message)
{
    NSLog(@"%s", message.c_str());
}

}

@implementation WCTDatabase (Monitor)

+ (void)globalTraceError:(WCTErrorTraceBlock)block
{
    [self globalTraceError:block withIdentifier:WCDB::NotifierLoggerName];
}

+ (void)globalTraceError:(WCTErrorTraceBlock)block
          withIdentifier:(const WCDB::String &)identifier
{
    WCDB::Console::initialize();

    if (block != nil) {
        WCDB::Notifier::shared()->setNotification(std::numeric_limits<int>::min(), identifier, [block](const WCDB::Error &error) {
            WCTError *nsError = [[WCTError alloc] initWithError:error];
            block(nsError);
        });
    } else {
        WCDB::Notifier::shared()->unsetNotification(identifier);
    }
}

+ (void)globalTracePerformance:(WCTPerformanceTraceBlock)trace
{
    WCDB::Handle::PerformanceNotification callback = nullptr;
    if (trace != nil) {
        callback = [trace](const WCDB::HandleNotification::Footprints &footprints, const int64_t &cost) {
            NSMutableArray<WCTPerformanceFootprint *> *array = [[NSMutableArray<WCTPerformanceFootprint *> alloc] init];
            for (const auto &footprint : footprints) {
                NSString *sql = [NSString stringWithUTF8String:footprint.sql.c_str()];
                [array addObject:[[WCTPerformanceFootprint alloc] initWithSQL:sql andFrequency:footprint.frequency]];
            }
            trace(array, (NSUInteger) cost);
        };
    }
    WCDB::Core::shared()->setNotificationWhenPerformanceGlobalTraced(callback);
}

+ (void)globalTraceSQL:(WCTSQLTraceBlock)trace
{
    WCDB::Handle::SQLNotification callback = nullptr;
    if (trace != nil) {
        callback = [trace](const WCDB::String &sql) {
            trace([NSString stringWithUTF8String:sql.c_str()]);
        };
    }
    WCDB::Core::shared()->setNotificationForSQLGLobalTraced(callback);
}

- (void)tracePerformance:(WCTPerformanceTraceBlock)trace
{
    WCDB::Handle::PerformanceNotification callback = nullptr;
    if (trace != nil) {
        callback = [trace](const WCDB::HandleNotification::Footprints &footprints, const int64_t &cost) {
            NSMutableArray<WCTPerformanceFootprint *> *array = [[NSMutableArray<WCTPerformanceFootprint *> alloc] init];
            for (const auto &footprint : footprints) {
                NSString *sql = [NSString stringWithUTF8String:footprint.sql.c_str()];
                [array addObject:[[WCTPerformanceFootprint alloc] initWithSQL:sql andFrequency:footprint.frequency]];
            }
            trace(array, (NSUInteger) cost);
        };
        _database->setConfig(WCDB::PerformanceTraceConfigName,
                             std::shared_ptr<WCDB::Config>(new WCDB::PerformanceTraceConfig(callback)),
                             WCDB::Configs::Priority::Highest);
    } else {
        _database->removeConfig(WCDB::PerformanceTraceConfigName);
    }
}

- (void)traceSQL:(WCTSQLTraceBlock)trace
{
    WCDB::Handle::SQLNotification callback = nullptr;
    if (trace != nil) {
        callback = [trace](const WCDB::String &sql) {
            trace([NSString stringWithUTF8String:sql.c_str()]);
        };
        _database->setConfig(WCDB::SQLTraceConfigName,
                             std::shared_ptr<WCDB::Config>(new WCDB::SQLTraceConfig(callback)),
                             WCDB::Configs::Priority::Highest);
    } else {
        _database->removeConfig(WCDB::SQLTraceConfigName);
    }
}

@end

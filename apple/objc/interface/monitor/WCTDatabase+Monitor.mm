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
#import <WCDB/Core.hpp>
#import <WCDB/WCTDatabase+Monitor.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTFoundation.h>
#import <WCDB/WCTPerformanceFootprint.h>

namespace WCDB {

void Core::print(const UnsafeStringView &message)
{
    NSLog(@"%s", message.data());
}

}

@implementation WCTDatabase (Monitor)

+ (void)globalTraceError:(WCTErrorTraceBlock)block
{
    WCDB::Core::shared().setNotificationWhenErrorTraced([block](const WCDB::Error &error) {
        WCTError *nsError = [[WCTError alloc] initWithError:error];
        block(nsError);
    });
}

+ (void)globalTracePerformance:(WCTPerformanceTraceBlock)trace
{
    WCDB::Handle::PerformanceNotification callback = nullptr;
    if (trace != nil) {
        callback = [trace](const WCDB::HandleNotification::Footprints &footprints, double cost) {
            NSMutableArray<WCTPerformanceFootprint *> *array = [NSMutableArray<WCTPerformanceFootprint *> arrayWithCapacity:footprints.size()];
            for (const auto &footprint : footprints) {
                NSString *sql = [NSString stringWithView:footprint.sql];
                [array addObject:[[WCTPerformanceFootprint alloc] initWithSQL:sql andFrequency:footprint.frequency]];
            }
            trace(array, cost);
        };
    }
    WCDB::Core::shared().setNotificationWhenPerformanceGlobalTraced(callback);
}

+ (void)globalTraceSQL:(WCTSQLTraceBlock)trace
{
    WCDB::Handle::SQLNotification callback = nullptr;
    if (trace != nil) {
        callback = [trace](const WCDB::UnsafeStringView &sql) {
            trace([NSString stringWithView:sql]);
        };
    }
    WCDB::Core::shared().setNotificationForSQLGLobalTraced(callback);
}

- (void)tracePerformance:(WCTPerformanceTraceBlock)trace
{
    if (trace != nil) {
        WCDB::Handle::PerformanceNotification callback = [trace](const WCDB::HandleNotification::Footprints &footprints, double cost) {
            NSMutableArray<WCTPerformanceFootprint *> *array = [NSMutableArray<WCTPerformanceFootprint *> arrayWithCapacity:footprints.size()];
            for (const auto &footprint : footprints) {
                NSString *sql = [NSString stringWithView:footprint.sql];
                [array addObject:[[WCTPerformanceFootprint alloc] initWithSQL:sql andFrequency:footprint.frequency]];
            }
            trace(array, cost);
        };
        _database->setConfig(WCDB::PerformanceTraceConfigName,
                             std::static_pointer_cast<WCDB::Config>(std::make_shared<WCDB::PerformanceTraceConfig>(callback)),
                             WCDB::Configs::Priority::Highest);
    } else {
        _database->removeConfig(WCDB::PerformanceTraceConfigName);
    }
}

- (void)traceSQL:(WCTSQLTraceBlock)trace
{
    if (trace != nil) {
        WCDB::Handle::SQLNotification callback = [trace](const WCDB::UnsafeStringView &sql) {
            trace([NSString stringWithView:sql]);
        };
        _database->setConfig(WCDB::SQLTraceConfigName,
                             std::static_pointer_cast<WCDB::Config>(std::make_shared<WCDB::SQLTraceConfig>(callback)),
                             WCDB::Configs::Priority::Highest);
    } else {
        _database->removeConfig(WCDB::SQLTraceConfigName);
    }
}

@end

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

#import "Assertion.hpp"
#import "CommonCore.hpp"
#import "Console.hpp"
#import "DBOperationNotifier.hpp"
#import "WCTConvertible.h"
#import "WCTDatabase+Monitor.h"
#import "WCTDatabase+Private.h"
#import "WCTError+Private.h"
#import "WCTFoundation.h"
#import "WCTPerformanceInfo+Private.h"

namespace WCDB {

void CommonCore::print(const UnsafeStringView& message)
{
#if WCDB_DEBUG
    NSLog(@"%s", message.data());
#endif
}

} // namespace WCDB

NSString* const WCTDatabaseMonitorInfoKeyHandleCount = [NSString stringWithUTF8String:WCDB::k_MonitorInfoKeyHandleCount];
NSString* const WCTDatabaseMonitorInfoKeyHandleOpenTime = [NSString stringWithUTF8String:WCDB::k_MonitorInfoKeyHandleOpenTime];
NSString* const WCTDatabaseMonitorInfoKeyHandleOpenCPUTime = [NSString stringWithUTF8String:WCDB::k_MonitorInfoKeyHandleOpenCPUTime];
NSString* const WCTDatabaseMonitorInfoKeySchemaUsage = [NSString stringWithUTF8String:WCDB::k_MonitorInfoKeySchemaUsage];
NSString* const WCTDatabaseMonitorInfoKeyTableCount = [NSString stringWithUTF8String:WCDB::k_MonitorInfoKeyTableCount];
NSString* const WCTDatabaseMonitorInfoKeyIndexCount = [NSString stringWithUTF8String:WCDB::k_MonitorInfoKeyIndexCount];
NSString* const WCTDatabaseMonitorInfoKeyTriggerCount = [NSString stringWithUTF8String:WCDB::k_MonitorInfoKeyTriggerCount];

@implementation WCTDatabase (Monitor)

+ (void)globalTraceError:(WCTErrorTraceBlock)block
{
    if (block != nil) {
        WCDB::CommonCore::shared().setNotificationWhenErrorTraced([block](const WCDB::Error& error) {
            WCTError* nsError = [[WCTError alloc] initWithError:error];
            block(nsError);
        });
    } else {
        WCDB::CommonCore::shared().setNotificationWhenErrorTraced(nullptr);
    }
}

- (void)traceError:(nullable WCDB_ESCAPE WCTErrorTraceBlock)block
{
    if (block != nil) {
        WCDB::CommonCore::shared().setNotificationWhenErrorTraced(self.path, [block](const WCDB::Error& error) {
            WCTError* nsError = [[WCTError alloc] initWithError:error];
            block(nsError);
        });
    } else {
        WCDB::CommonCore::shared().setNotificationWhenErrorTraced(self.path, nullptr);
    }
}

+ (void)globalTracePerformance:(WCTPerformanceTraceBlock)trace
{
    WCDB::InnerHandle::PerformanceNotification callback = nullptr;
    if (trace != nil) {
        callback = [trace](const WCDB::Tag& tag, const WCDB::UnsafeStringView& path, const void* handle, const WCDB::UnsafeStringView& sql, const WCDB::InnerHandle::PerformanceInfo& info) {
            WCTPerformanceInfo* nsInfo = [[WCTPerformanceInfo alloc] initWithPerformanceInfo:info];
            trace(tag, [NSString stringWithUTF8String:path.data()], (uint64_t) handle, [NSString stringWithUTF8String:sql.data()], nsInfo);
        };
    }
    WCDB::CommonCore::shared().setNotificationWhenPerformanceGlobalTraced(callback);
}

- (void)tracePerformance:(WCTPerformanceTraceBlock)trace
{
    if (trace != nil) {
        WCDB::InnerHandle::PerformanceNotification callback = [trace](const WCDB::Tag& tag, const WCDB::UnsafeStringView& path, const void* handle, const WCDB::UnsafeStringView& sql, const WCDB::InnerHandle::PerformanceInfo& info) {
            WCTPerformanceInfo* nsInfo = [[WCTPerformanceInfo alloc] initWithPerformanceInfo:info];
            trace(tag, [NSString stringWithUTF8String:path.data()], (uint64_t) handle, [NSString stringWithUTF8String:sql.data()], nsInfo);
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
        WCDB::InnerHandle::SQLNotification callback = [trace](const WCDB::Tag& tag,
                                                              const WCDB::UnsafeStringView& path,
                                                              const void* handle,
                                                              const WCDB::UnsafeStringView& sql,
                                                              const WCDB::UnsafeStringView& info) {
            trace(tag, [NSString stringWithUTF8String:path.data()], (uint64_t) handle, [NSString stringWithView:sql], info.length() > 0 ? [NSString stringWithView:info.data()] : nil);
        };
        _database->setConfig(WCDB::SQLTraceConfigName,
                             std::static_pointer_cast<WCDB::Config>(std::make_shared<WCDB::SQLTraceConfig>(callback)),
                             WCDB::Configs::Priority::Highest);
    } else {
        _database->removeConfig(WCDB::SQLTraceConfigName);
    }
}

+ (void)globalTraceSQL:(WCTSQLTraceBlock)trace
{
    WCDB::InnerHandle::SQLNotification callback = nullptr;
    if (trace != nil) {
        callback = [trace](const WCDB::Tag& tag,
                           const WCDB::UnsafeStringView& path,
                           const void* handle,
                           const WCDB::UnsafeStringView& sql,
                           const WCDB::UnsafeStringView& info) {
            trace(tag, [NSString stringWithUTF8String:path.data()], (uint64_t) handle, [NSString stringWithView:sql], info.length() > 0 ? [NSString stringWithView:info.data()] : nil);
        };
    }
    WCDB::CommonCore::shared().setNotificationForSQLGLobalTraced(callback);
}

- (void)enableFullSQLTrace:(BOOL)enable
{
    _database->setFullSQLTraceEnable(enable);
}

+ (void)globalTraceDatabaseOperation:(nullable WCDB_ESCAPE WCTDatabaseOperationTraceBlock)trace
{
    if (trace != nil) {
        WCDB::DBOperationNotifier::shared().setNotification([=](WCDB::InnerDatabase* innerDatabase, WCDB::DBOperationNotifier::Operation operation, const WCDB::StringViewMap<WCDB::Value>& info) {
            WCTDatabase* database = [[WCTDatabase alloc] initWithUnsafeDatabase:innerDatabase];
            NSMutableDictionary* nsInfo = [[NSMutableDictionary alloc] init];
            for (auto iter = info.begin(); iter != info.end(); iter++) {
                NSString* key = [NSString stringWithUTF8String:iter->first.data()];
                switch (iter->second.getType()) {
                case WCDB::Value::Type::Integer: {
                    nsInfo[key] = @(iter->second.intValue());
                } break;
                case WCDB::Value::Type::Float: {
                    nsInfo[key] = @(iter->second.floatValue());
                } break;
                case WCDB::Value::Type::Text: {
                    nsInfo[key] = [NSString stringWithUTF8String:iter->second.textValue().data()];
                } break;
                case WCDB::Value::Type::BLOB: {
                    const WCDB::Data& value = iter->second.blobValue();
                    nsInfo[key] = [NSData dataWithBytes:value.buffer() length:value.size()];
                } break;
                default:
                    break;
                }
            }
            trace(database, (WCTDatabaseOperation) operation, nsInfo);
        });
    } else {
        WCDB::DBOperationNotifier::shared().setNotification(nil);
    }
}

+ (void)globalTraceBusy:(nullable WCDB_ESCAPE WCTDatabaseBusyTraceBlock)trace withTimeOut:(double)timeOut
{
    if (trace != nil && timeOut > 0) {
        WCDB::CommonCore::shared().setBusyMonitor([=](const WCDB::Tag& tag, const WCDB::UnsafeStringView& path, uint64_t tid, const WCDB::UnsafeStringView& sql) {
            trace(tag, [NSString stringWithUTF8String:path.data()], tid, [NSString stringWithUTF8String:sql.data()]);
        },
                                                  timeOut);
    } else {
        WCDB::CommonCore::shared().setBusyMonitor(nullptr, 0);
    }
}

@end

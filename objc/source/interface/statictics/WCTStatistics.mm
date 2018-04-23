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

#import <WCDB/Interface.h>
#import <WCDB/WCTCoreError+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTFileError+Private.h>
#import <WCDB/WCTHandleError+Private.h>
#import <WCDB/WCTSQLiteError+Private.h>

@implementation WCTStatistics

+ (void)SetGlobalErrorReport:(WCTErrorReportBlock)block
{
    WCDB::Error::Report::Callback callback = nullptr;
    if (block) {
        callback = [block](const WCDB::Error &error) {
            WCTError *nsError = nil;
            switch (error.getType()) {
                case WCDB::HandleError::type:
                    nsError = [[WCTHandleError alloc] initWithHandleError:(const WCDB::HandleError &) error];
                    break;
                case WCDB::FileError::type:
                    nsError = [[WCTFileError alloc] initWithFileError:(const WCDB::FileError &) error];
                    break;
                case WCDB::CoreError::type:
                    nsError = [[WCTCoreError alloc] initWithCoreError:(const WCDB::CoreError &) error];
                    break;
                case WCDB::SQLiteError::type:
                    nsError = [[WCTSQLiteError alloc] initWithSQLiteError:(const WCDB::SQLiteError &) error];
                    break;
                default:
                    nsError = [[WCTError alloc] initWithError:error];
                    break;
            }
            block(nsError);
        };
    }
    WCDB::Error::Report::shared()->setCallback(callback);
}

+ (void)SetGlobalPerformanceTrace:(WCTPerformanceTraceBlock)trace
{
    WCDB::Handle::PerformanceTraceCallback callback = nullptr;
    if (trace) {
        callback = [trace](WCDB::Database::Tag tag, const WCDB::Tracer::Footprints &footprints, const int64_t &cost) {
            NSMutableArray<WCTPerformanceFootprint *> *array = [[NSMutableArray<WCTPerformanceFootprint *> alloc] init];
            for (const auto &footprint : footprints) {
                NSString *sql = [NSString stringWithCppString:footprint.sql];
                [array addObject:[[WCTPerformanceFootprint alloc] initWithSQL:sql andFrequency:footprint.frequency]];
            }
            trace(tag, array, (NSUInteger) cost);
        };
    }
    WCDB::BuiltinConfig::shared()->setGlobalPerformanceTrace(callback);
}

+ (void)SetGlobalSQLTrace:(WCTSQLTraceBlock)trace
{
    WCDB::Handle::SQLTraceCallback callback = nullptr;
    if (trace) {
        callback = [trace](const std::string &sql) {
            trace([NSString stringWithCppString:sql]);
        };
    }
    WCDB::BuiltinConfig::shared()->setGlobalSQLTrace(callback);
}

+ (void)ResetGlobalErrorReport
{
    WCDB::Error::Report::shared()->setCallback(WCDB::Error::Report::defaultCallback);
}

@end

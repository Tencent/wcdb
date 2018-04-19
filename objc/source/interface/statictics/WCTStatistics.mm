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
    WCDB::Error::Report *report = WCDB::Error::Report::sharedReport();
    if (report) {
        report->setCallback([block](const WCDB::Error &error) {
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
        });
    } else {
        report->setCallback(nullptr);
    }
}

+ (void)SetGlobalPerformanceTrace:(WCTPerformanceTraceBlock)trace
{
    if (trace) {
        WCDB::BuiltinConfig::SetGlobalPerformanceTrace([trace](WCDB::Database::Tag tag, const WCDB::Tracer::Footprints &footprints, const int64_t &cost) {
            NSMutableArray<WCTPerformanceFootprint *> *array = [[NSMutableArray<WCTPerformanceFootprint *> alloc] init];
            for (const auto &footprint : footprints) {
                [array addObject:[[WCTPerformanceFootprint alloc] initWithSQL:@(footprint.sql.c_str()) andFrequency:footprint.frequency]];
            }
            trace(tag, array, (NSUInteger) cost);
        });
    } else {
        WCDB::BuiltinConfig::SetGlobalPerformanceTrace(nullptr);
    }
}

+ (void)SetGlobalSQLTrace:(WCTSQLTraceBlock)trace
{
    if (trace) {
        WCDB::BuiltinConfig::SetGlobalSQLTrace([trace](const std::string &sql) {
            trace(@(sql.c_str()));
        });
    } else {
        WCDB::BuiltinConfig::SetGlobalSQLTrace(nullptr);
    }
}

+ (void)ResetGlobalErrorReport
{
    WCDB::Error::Report::sharedReport()->resetCallback();
}

@end

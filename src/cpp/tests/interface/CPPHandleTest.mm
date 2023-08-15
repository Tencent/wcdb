//
// Created by 陈秋文 on 2023/8/13.
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

#import "CPPTestCase.h"
#import <Foundation/Foundation.h>

@interface CPPHandleTests : CPPTableTestCase

@end

@implementation CPPHandleTests

- (void)test_cancellation_signal
{
    TestCaseAssertTrue([self createValueTable]);
    WCDB::MultiRowsValue rows = [Random.shared testCaseValuesWithCount:10000 startingFromIdentifier:0];
    TestCaseAssertTrue(self.database->insertRows(rows, self.columns, self.tableName.UTF8String));
    bool hasTestInterrupt = false;
    self.database->traceError([&](const WCDB::Error &error) {
        if (error.level != WCDB::Error::Level::Error) {
            return;
        }
        XCTAssertTrue(error.code() == WCDB::Error::Code::Interrupt);
        hasTestInterrupt = true;
    });
    WCDB::Handle::CancellationSignal signal;
    [self.dispatch async:^{
        WCDB::Handle handle = self.database->getHandle();
        handle.attachCancellationSignal(signal);
        auto allRows = handle.selectAllRow(self.columns, self.tableName.UTF8String);
        XCTAssertFalse(allRows.succeed());
        handle.invalidate();
    }];
    usleep(10000);
    signal.cancel();
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue(hasTestInterrupt);
    self.database->traceSQL(nullptr);
}

- (void)test_write_with_handle_count_limit
{
    int maxHandleCount = 0;
    WCDB::Database::globalTraceDatabaseOperation([&](WCDB::Database &,
                                                     WCDB::Database::Operation operationType,
                                                     WCDB::StringViewMap<WCDB::Value> &info) {
        if (operationType != WCDB::Database::Operation::OpenHandle) {
            return;
        }
        auto iter = info.find(WCDB::Database::MonitorInfoKeyHandleCount);
        if (iter != info.end()) {
            maxHandleCount = std::max(maxHandleCount, (int) iter->second.intValue());
        }
    });
    TestCaseAssertTrue([self createObjectTable]);
    auto table = self.table;
    for (int i = 0; i < 40; i++) {
        [self.dispatch async:^{
            switch (i % 4) {
            case 0: {
                auto objects = [Random.shared testCaseObjectsWithCount:100 startingFromIdentifier:i * 100];
                TestCaseAssertTrue(self.table.insertObjects(objects));
            } break;
            case 1: {
                TestCaseAssertTrue(self.table.updateRow("abc", WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) > (i - 1) * 100 && WCDB_FIELD(CPPTestCaseObject::identifier) <= i * 100));
            } break;
            case 2: {
                TestCaseAssertTrue(self.table.deleteObjects(WCDB_FIELD(CPPTestCaseObject::identifier) > ((i - 2) * 100 + 50) && WCDB_FIELD(CPPTestCaseObject::identifier) <= i * 100));
            } break;
            case 3: {
                TestCaseAssertTrue(self.database->runTransaction([&](WCDB::Handle &) {
                    return self.table.updateRow("abc2", WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) > (i - 1) * 100 && WCDB_FIELD(CPPTestCaseObject::identifier) <= i * 100);
                }));
            } break;

            default:
                break;
            }
            usleep(100000);
        }];
    }
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue(maxHandleCount <= 4);
    WCDB::Database::globalTraceDatabaseOperation(nullptr);
}

- (void)test_read_with_handle_count_limit
{
    int maxHandleCount = 0;
    WCDB::Database::globalTraceDatabaseOperation([&](WCDB::Database &,
                                                     WCDB::Database::Operation operationType,
                                                     WCDB::StringViewMap<WCDB::Value> &info) {
        if (operationType != WCDB::Database::Operation::OpenHandle) {
            return;
        }
        auto iter = info.find(WCDB::Database::MonitorInfoKeyHandleCount);
        if (iter != info.end()) {
            maxHandleCount = std::max(maxHandleCount, (int) iter->second.intValue());
        }
    });
    TestCaseAssertTrue([self createObjectTable]);
    auto objects = [Random.shared testCaseObjectsWithCount:32000 startingFromIdentifier:0];
    TestCaseAssertTrue(self.table.insertObjects(objects));

    for (int i = 0; i < 320; i++) {
        [self.dispatch async:^{
            switch (i % 5) {
            case 0: {
                auto ret = self.table.getAllObjects(WCDB::Expression(), WCDB::OrderingTerms(), 100, i * 100);
                TestCaseAssertTrue(ret.hasValue() && ret.value().size() == 100);
            } break;
            case 1: {
                auto value = self.database->getValueFromStatement(WCDB::StatementSelect().select(CPPTestCaseObject::allFields().count()).from(self.tableName.UTF8String));
                TestCaseAssertTrue(value.hasValue() && value.value().intValue() == 32000);
            } break;
            case 2: {
                auto column = self.table.selectOneColumn(WCDB_FIELD(CPPTestCaseObject::content), WCDB::Expression(), WCDB::OrderingTerms(), 100, i * 100);
                TestCaseAssertTrue(column.hasValue() && column.value().size() == 100);
            } break;
            case 3: {
                auto rows = self.table.selectAllRow(CPPTestCaseObject::allFields(), WCDB::Expression(), WCDB::OrderingTerms(), 100, i * 100);
                TestCaseAssertTrue(rows.hasValue() && rows.value().size() == 100);
            } break;
            case 4: {
                auto row = self.table.selectOneRow(CPPTestCaseObject::allFields(), WCDB::Expression(), WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
                TestCaseAssertTrue(row.hasValue() && row.value().size() == 2);
            } break;

            default:
                break;
            }

            usleep(100000);
        }];
    }

    [self.dispatch waitUntilDone];
    TestCaseAssertTrue(maxHandleCount > 4 && maxHandleCount <= 32);
    WCDB::Database::globalTraceDatabaseOperation(nullptr);
}

- (void)test_read_write_with_handle_count_limit
{
    int maxHandleCount = 0;
    WCDB::Database::globalTraceDatabaseOperation([&](WCDB::Database &,
                                                     WCDB::Database::Operation operationType,
                                                     WCDB::StringViewMap<WCDB::Value> &info) {
        if (operationType != WCDB::Database::Operation::OpenHandle) {
            return;
        }
        auto iter = info.find(WCDB::Database::MonitorInfoKeyHandleCount);
        if (iter != info.end()) {
            maxHandleCount = std::max(maxHandleCount, (int) iter->second.intValue());
        }
    });

    TestCaseAssertTrue([self createObjectTable]);
    auto objects = [Random.shared testCaseObjectsWithCount:32000 startingFromIdentifier:0];
    TestCaseAssertTrue(self.table.insertObjects(objects));

    for (int i = 0; i < 320; i++) {
        if (i % 8 == 0) {
            [self.dispatch async:^{
                auto newObjects = [Random.shared testCaseObjectsWithCount:100 startingFromIdentifier:i * 100 + 32000];
                TestCaseAssertTrue(self.table.insertObjects(newObjects));
                usleep(100000);
            }];
        } else {
            [self.dispatch async:^{
                auto ret = self.table.getAllObjects(WCDB::Expression(), WCDB::OrderingTerms(), 100, i * 100);
                TestCaseAssertTrue(ret.hasValue() && ret.value().size() == 100);
                usleep(100000);
            }];
        }
    }

    [self.dispatch waitUntilDone];
    TestCaseAssertTrue(maxHandleCount > 4 && maxHandleCount <= 32);
    WCDB::Database::globalTraceDatabaseOperation(nullptr);
}

@end

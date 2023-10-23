//
// Created by qiuwenchen on 2022/8/13.
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

@interface CPPTraceTests : CPPTableTestCase

@end

@implementation CPPTraceTests

- (void)test_trace_sql
{
    WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion());

    BOOL tested = NO;
    self.database->traceSQL([&](long tag, const WCDB::UnsafeStringView &path, const void *, const WCDB::UnsafeStringView &sql, const WCDB::UnsafeStringView &) {
        XCTAssertEqual(tag, self.database->getTag());
        XCTAssertTrue(path.equal(self.database->getPath()));
        if (strcmp(sql.data(), statement.getDescription().data()) == 0) {
            tested = YES;
        }
    });
    TestCaseAssertTrue(self.database->execute(statement));
    TestCaseAssertTrue(tested);

    self.database->traceSQL(nullptr);
}

- (void)test_trace_performance
{
    TestCaseAssertTrue([self createObjectTable]);

    std::vector<CPPTestCaseObject> objects;
    for (int i = 0; i < 1000; i++) {
        objects.emplace_back(0, [Random.shared stringWithLength:4096].UTF8String);
        objects.back().isAutoIncrement = true;
    }
    int testCount = 0;
    self.database->tracePerformance(nullptr);
    self.database->tracePerformance([&](long tag, const WCDB::UnsafeStringView &path, uint64_t, const WCDB::UnsafeStringView &sql, const WCDB::Database::PerformanceInfo &info) {
        XCTAssertEqual(tag, self.database->getTag());
        XCTAssertTrue(path.equal(self.database->getPath()));
        if (sql.hasPrefix("COMMIT")) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount > 0);
            XCTAssertTrue(info.indexPageWriteCount == 0);
            XCTAssertTrue(info.overflowPageWriteCount > 0);
            XCTAssertTrue(info.tablePageReadCount == 0);
            XCTAssertTrue(info.indexPageReadCount == 0);
            XCTAssertTrue(info.overflowPageReadCount == 0);
            testCount++;
        } else if (sql.hasPrefix("CREATE INDEX")) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount == 1);
            XCTAssertTrue(info.indexPageWriteCount > 0);
            XCTAssertTrue(info.overflowPageWriteCount == objects.size());
            XCTAssertTrue(info.tablePageReadCount > 0);
            XCTAssertTrue(info.indexPageReadCount >= 0);
            XCTAssertTrue(info.overflowPageReadCount == objects.size());
            testCount++;
        } else if (sql.hasPrefix("SELECT")) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount == 0);
            XCTAssertTrue(info.indexPageWriteCount == 0);
            XCTAssertTrue(info.overflowPageWriteCount == 0);
            testCount++;
            if (sql.hasSuffix("ORDER BY content")) {
                XCTAssertTrue(info.tablePageReadCount == 0);
                XCTAssertTrue(info.indexPageReadCount > 0);
                XCTAssertTrue(info.overflowPageReadCount == objects.size());
            } else {
                XCTAssertTrue(info.tablePageReadCount > 0);
                XCTAssertTrue(info.indexPageReadCount == 0);
                XCTAssertTrue(info.overflowPageReadCount == objects.size());
            }
        }
    });
    TestCaseAssertTrue(self.table.insertObjects(objects));
    TestCaseAssertTrue(self.database->execute(WCDB::StatementCreateIndex().createIndex("testIndex").table(self.tableName).indexed(WCDB_FIELD(CPPTestCaseObject::content))));
    TestCaseAssertTrue(self.table.getAllObjects().value().size() == objects.size());
    TestCaseAssertTrue(self.table.getAllObjects(WCDB::Expression(), WCDB_FIELD(CPPTestCaseObject::content)).value().size() == objects.size());
    TestCaseAssertTrue(testCount == 4);
    self.database->tracePerformance(nil);
}

- (void)test_global_trace_error
{
    BOOL tested = NO;
    WCDB::Database::globalTraceError(nullptr);
    WCDB::Database::globalTraceError([&](const WCDB::Error &error) {
        if (error.level == WCDB::Error::Level::Error
            && strcmp(error.getPath().data(), self.path.UTF8String) == 0
            && error.getTag().value() == self.database->getTag()
            && error.code() == WCDB::Error::Code::Error
            && strcmp(error.getSQL().data(), "SELECT 1 FROM dummy") == 0) {
            tested = YES;
        }
    });

    TestCaseAssertTrue(self.database->canOpen());
    TestCaseAssertFalse(self.database->execute(WCDB::StatementSelect().select(1).from("dummy")));
    TestCaseAssertTrue(tested);
}

- (void)test_trace_error
{
    BOOL tested = NO;
    self.database->traceError([&](const WCDB::Error &error) {
        TestCaseAssertCPPStringEqual(error.getPath().data(), self.path.UTF8String);
        TestCaseAssertEqual(error.getTag().value(), self.database->getTag());
        if (error.level == WCDB::Error::Level::Error
            && error.code() == WCDB::Error::Code::Error
            && strcmp(error.getSQL().data(), "SELECT 1 FROM dummy") == 0) {
            tested = YES;
        }
    });

    TestCaseAssertTrue(self.database->canOpen());
    TestCaseAssertFalse(self.database->execute(WCDB::StatementSelect().select(1).from("dummy")));
    TestCaseAssertTrue(tested);
}

- (void)test_global_trace_sql
{
    WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion());

    BOOL tested = NO;
    WCDB::Database::globalTraceSQL([&](long tag, const WCDB::UnsafeStringView &path, const void *, const WCDB::UnsafeStringView &sql, const WCDB::UnsafeStringView &) {
        if (!path.equal(self.database->getPath())) {
            return;
        }
        XCTAssertEqual(tag, self.database->getTag());
        if (strcmp(sql.data(), statement.getDescription().data()) == 0) {
            tested = YES;
        }
    });
    TestCaseAssertTrue(self.database->execute(statement));
    TestCaseAssertTrue(tested);

    WCDB::Database::globalTraceError(nullptr);
}

- (void)test_global_trace_performance
{
    std::vector<CPPTestCaseObject> objects;
    for (int i = 0; i < 1000; i++) {
        objects.emplace_back(0, [Random.shared stringWithLength:4096].UTF8String);
        objects.back().isAutoIncrement = true;
    }
    int testCount = 0;
    bool lastSQLIsInsert = false;
    WCDB::Database::globalTracePerformance([&](long tag, const WCDB::UnsafeStringView &path, uint64_t, const WCDB::UnsafeStringView &sql, const WCDB::Database::PerformanceInfo &info) {
        if (!path.equal(self.database->getPath())) {
            return;
        }
        XCTAssertEqual(tag, self.database->getTag());
        XCTAssertTrue(path.equal(self.database->getPath()));
        if (sql.hasPrefix("COMMIT") && lastSQLIsInsert) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount > 0);
            XCTAssertTrue(info.indexPageWriteCount == 0);
            XCTAssertTrue(info.overflowPageWriteCount > 0);
            XCTAssertTrue(info.tablePageReadCount == 0);
            XCTAssertTrue(info.indexPageReadCount == 0);
            XCTAssertTrue(info.overflowPageReadCount == 0);
            testCount++;
        } else if (sql.hasPrefix("CREATE INDEX")) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount == 1);
            XCTAssertTrue(info.indexPageWriteCount > 0);
            XCTAssertTrue(info.overflowPageWriteCount == objects.size());
            XCTAssertTrue(info.tablePageReadCount > 0);
            XCTAssertTrue(info.indexPageReadCount >= 0);
            XCTAssertTrue(info.overflowPageReadCount == objects.size());
            testCount++;
        } else if (sql.hasPrefix("SELECT")) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount == 0);
            XCTAssertTrue(info.indexPageWriteCount == 0);
            XCTAssertTrue(info.overflowPageWriteCount == 0);
            testCount++;
            if (sql.hasSuffix("ORDER BY content")) {
                XCTAssertTrue(info.tablePageReadCount == 0);
                XCTAssertTrue(info.indexPageReadCount > 0);
                XCTAssertTrue(info.overflowPageReadCount == objects.size());
            } else {
                XCTAssertTrue(info.tablePageReadCount > 0);
                XCTAssertTrue(info.indexPageReadCount == 0);
                XCTAssertTrue(info.overflowPageReadCount == objects.size());
            }
        }
        lastSQLIsInsert = sql.hasPrefix("INSERT");
    });
    TestCaseAssertTrue([self createObjectTable]);
    TestCaseAssertTrue(self.table.insertObjects(objects));
    TestCaseAssertTrue(self.database->execute(WCDB::StatementCreateIndex().createIndex("testIndex").table(self.tableName).indexed(WCDB_FIELD(CPPTestCaseObject::content))));
    TestCaseAssertTrue(self.table.getAllObjects().value().size() == objects.size());
    TestCaseAssertTrue(self.table.getAllObjects(WCDB::Expression(), WCDB_FIELD(CPPTestCaseObject::content)).value().size() == objects.size());
    TestCaseAssertTrue(testCount == 4);
    WCDB::Database::globalTracePerformance(nil);
}

- (void)test_trace_db_operation
{
    long tag = 0;
    WCDB::StringView path;
    int openHandleCount = 0;
    int tableCount = 0;
    int indexCount = 0;
    WCDB::Database::globalTraceDatabaseOperation([&](WCDB::Database &database,
                                                     WCDB::Database::Operation operation,
                                                     WCDB::StringViewMap<WCDB::Value> &info) {
        switch (operation) {
        case WCDB::Database::Operation::Create:
            path = database.getPath();
            break;
        case WCDB::Database::Operation::SetTag:
            tag = database.getTag();
            break;
        case WCDB::Database::Operation::OpenHandle: {
            openHandleCount++;
            TestCaseAssertTrue(info[WCDB::Database::MonitorInfoKeyHandleCount].intValue() == 1);
            TestCaseAssertTrue(info[WCDB::Database::MonitorInfoKeyHandleOpenTime].intValue() > 0);
            TestCaseAssertTrue(info[WCDB::Database::MonitorInfoKeyHandleOpenCPUTime].intValue() > 0);
            TestCaseAssertTrue(info[WCDB::Database::MonitorInfoKeySchemaUsage].intValue() > 0);
            TestCaseAssertTrue(info[WCDB::Database::MonitorInfoKeyTriggerCount].intValue() == 0);
            tableCount = (int) info[WCDB::Database::MonitorInfoKeyTableCount].intValue();
            indexCount = (int) info[WCDB::Database::MonitorInfoKeyIndexCount].intValue();
        } break;
        }
    });
    TestCaseAssertTrue([self createValueTable]);
    TestCaseAssertTrue(self.database->execute(WCDB::StatementCreateIndex()
                                              .createIndex("testIndex")
                                              .table(self.tableName)
                                              .indexed(WCDB_FIELD(CPPTestCaseObject::content))));

    TestCaseAssertTrue(tag = self.database->getTag());
    TestCaseAssertCPPStringEqual(path.data(), self.database->getPath().data());
    TestCaseAssertTrue(openHandleCount == 1);

    self.database->close();
    TestCaseAssertTrue(self.database->insertRows([Random.shared autoIncrementTestCaseValuesWithCount:10], self.columns, self.tableName.UTF8String));

    TestCaseAssertTrue(openHandleCount == 2);
    TestCaseAssertTrue(tableCount == 4);
    TestCaseAssertTrue(indexCount == 1);
    WCDB::Database::globalTraceDatabaseOperation(nullptr);
}

- (void)test_global_trace_busy
{
    uint64_t testTid = 0;
    WCDB::Database::globalTraceBusy([&](long tag, const WCDB::UnsafeStringView &path, uint64_t tid, const WCDB::UnsafeStringView &sql) {
        XCTAssertTrue(tag == self.database->getTag());
        TestCaseAssertCPPStringEqual(path.data(), self.database->getPath().data());
        TestCaseAssertCPPStringEqual(sql.data(), "INSERT INTO testTable(identifier, content) VALUES(?1, ?2)");
        testTid = tid;
    },
                                    0.1);

    XCTAssertTrue([self createObjectTable]);
    __block uint64_t dispatchTid = 0;
    auto objects = [Random.shared testCaseObjectsWithCount:100000 startingFromIdentifier:1];
    [self.dispatch async:^{
        pthread_threadid_np(nullptr, &dispatchTid);
        XCTAssertTrue(self.table.insertObjects(objects));
    }];
    usleep(100000);
    XCTAssertTrue(self.table.insertObjects([Random.shared autoIncrementTestCaseObject]));

    [self.dispatch waitUntilDone];
    XCTAssertTrue(testTid != 0 && testTid == dispatchTid);
    WCDB::Database::globalTraceBusy(nullptr, 0);
}

@end

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

#import "WTCAssert.h"
#import <WINQ/abstract.h>

@interface WTCStatementCreateVirtualTableTests : XCTestCase

@end

@implementation WTCStatementCreateVirtualTableTests

- (void)testStatementCreateVirtualTable
{
    std::string schemaName = "testSchema";
    std::string tableName = "testTable";
    std::string moduleName = "FTS3";
    WCDB::ModuleArgument moduleArgument1 = WCDB::ModuleArgument("tokenize", "WCDB");
    WCDB::ModuleArgument moduleArgument2 = WCDB::ModuleArgument(WCDB::ColumnDef("testColumn1").withType(WCDB::ColumnType::Integer32));
    std::list<WCDB::ModuleArgument> moduleArguments = {moduleArgument1, moduleArgument2};

    WINQAssertEqual(WCDB::StatementCreateVirtualTable()
                        .createVirtualTable(tableName, false)
                        .withSchema(schemaName)
                        .usingModule(moduleName, moduleArgument1),
                    @"CREATE VIRTUAL TABLE testSchema.testTable USING FTS3(tokenize=WCDB)");

    WINQAssertEqual(WCDB::StatementCreateVirtualTable()
                        .createVirtualTable(tableName, false)
                        .withSchema(schemaName)
                        .usingModule(moduleName, moduleArguments),
                    @"CREATE VIRTUAL TABLE testSchema.testTable USING FTS3(tokenize=WCDB, testColumn1 INTEGER)");

    WINQAssertEqual(WCDB::StatementCreateVirtualTable()
                        .createVirtualTable(tableName, false)
                        .withSchema(schemaName)
                        .usingModule(moduleName),
                    @"CREATE VIRTUAL TABLE testSchema.testTable USING FTS3");

    WINQAssertEqual(WCDB::StatementCreateVirtualTable()
                        .createVirtualTable(tableName, true)
                        .usingModule(moduleName, moduleArgument1),
                    @"CREATE VIRTUAL TABLE IF NOT EXISTS testTable USING FTS3(tokenize=WCDB)");
}

@end

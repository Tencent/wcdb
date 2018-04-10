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

#import "WINQTestCase.h"

@interface StatementCreateVirtualTableTests : WINQTestCase

@end

@implementation StatementCreateVirtualTableTests

- (void)testStatementCreateVirtualTable
{
    XCTAssertEqual(WCDB::StatementCreateVirtualTable().getType(), WCDB::Statement::Type::CreateVirtualTable);

    std::string moduleName = "fts3";

    WINQAssertEqual(WCDB::StatementCreateVirtualTable()
                        .createVirtualTable(self.class.tableName)
                        .ifNotExists(false)
                        .withSchema(self.class.schemaName)
                        .usingModule(moduleName)
                        .on(self.class.moduleArgument),
                    @"CREATE VIRTUAL TABLE testSchema.testTable USING fts3(tokenize=WCDB)");

    WINQAssertEqual(WCDB::StatementCreateVirtualTable()
                        .createVirtualTable(self.class.tableName)
                        .ifNotExists(false)
                        .withSchema(self.class.schemaName)
                        .usingModule(moduleName)
                        .on(self.class.moduleArguments),
                    @"CREATE VIRTUAL TABLE testSchema.testTable USING fts3(tokenize=WCDB, testColumn INTEGER)");

    WINQAssertEqual(WCDB::StatementCreateVirtualTable()
                        .createVirtualTable(self.class.tableName)
                        .ifNotExists(false)
                        .withSchema(self.class.schemaName)
                        .usingModule(moduleName),
                    @"CREATE VIRTUAL TABLE testSchema.testTable USING fts3");

    WINQAssertEqual(WCDB::StatementCreateVirtualTable()
                        .createVirtualTable(self.class.tableName)
                        .ifNotExists(false)
                        .usingModule(moduleName)
                        .on(self.class.moduleArgument),
                    @"CREATE VIRTUAL TABLE testTable USING fts3(tokenize=WCDB)");

    WINQAssertEqual(WCDB::StatementCreateVirtualTable()
                        .createVirtualTable(self.class.tableName)
                        .ifNotExists(true)
                        .withSchema(self.class.schemaName)
                        .usingModule(moduleName)
                        .on(self.class.moduleArgument),
                    @"CREATE VIRTUAL TABLE IF NOT EXISTS testSchema.testTable USING fts3(tokenize=WCDB)");

    //Default
    WINQAssertEqual(WCDB::StatementCreateVirtualTable()
                        .createVirtualTable(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .usingModule(moduleName)
                        .on(self.class.moduleArgument),
                    @"CREATE VIRTUAL TABLE IF NOT EXISTS testSchema.testTable USING fts3(tokenize=WCDB)");
}

@end

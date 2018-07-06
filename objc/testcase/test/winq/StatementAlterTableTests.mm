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

@interface StatementAlterTableTests : WINQTestCase

@end

@implementation StatementAlterTableTests

- (void)testStatementAlterTable
{
    XCTAssertEqual(WCDB::StatementAlterTable().getType(), WCDB::Statement::Type::AlterTable);

    WINQAssertEqual(WCDB::StatementAlterTable()
                    .alterTable(self.class.schemaName, self.class.tableName)
                    .renameTo("testNewTable"),
                    @"ALTER TABLE testSchema.testTable RENAME TO testNewTable");

    WINQAssertEqual(WCDB::StatementAlterTable()
                    .alterTable(self.class.schemaName, self.class.tableName)
                    .addColumn(self.class.columnDef),
                    @"ALTER TABLE testSchema.testTable ADD COLUMN testColumn INTEGER");

    WINQAssertEqual(WCDB::StatementAlterTable()
                    .alterTable(self.class.tableName)
                    .renameTo("testNewTable"),
                    @"ALTER TABLE main.testTable RENAME TO testNewTable");
}

@end

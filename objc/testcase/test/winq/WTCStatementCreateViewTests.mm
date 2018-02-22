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

#import "WTCWINQTestCase.h"

@interface WTCStatementCreateViewTests : WTCWINQTestCase

@end

@implementation WTCStatementCreateViewTests

- (void)testStatementCreateView
{
    WINQAssertEqual(WCDB::StatementCreateView()
                        .createView(self.class.viewName, false, false)
                        .as(self.class.statementSelect),
                    @"CREATE VIEW testView AS SELECT testColumn FROM testTable");

    WINQAssertEqual(WCDB::StatementCreateView()
                        .createView(self.class.viewName, false, false)
                        .on(self.class.column)
                        .as(self.class.statementSelect),
                    @"CREATE VIEW testView(testColumn) AS SELECT testColumn FROM testTable");

    WINQAssertEqual(WCDB::StatementCreateView()
                        .createView(self.class.viewName, false, false)
                        .on(self.class.columns)
                        .as(self.class.statementSelect),
                    @"CREATE VIEW testView(testColumn, testColumn2) AS SELECT testColumn FROM testTable");

    WINQAssertEqual(WCDB::StatementCreateView()
                        .createView(self.class.viewName, false, false)
                        .withSchema(self.class.schemaName)
                        .as(self.class.statementSelect),
                    @"CREATE VIEW testSchema.testView AS SELECT testColumn FROM testTable");

    WINQAssertEqual(WCDB::StatementCreateView()
                        .createView(self.class.viewName, true, false)
                        .as(self.class.statementSelect),
                    @"CREATE VIEW IF NOT EXISTS testView AS SELECT testColumn FROM testTable");

    WINQAssertEqual(WCDB::StatementCreateView()
                        .createView(self.class.viewName, false, true)
                        .as(self.class.statementSelect),
                    @"CREATE TEMP VIEW testView AS SELECT testColumn FROM testTable");

    //Default
    WINQAssertEqual(WCDB::StatementCreateView()
                        .createView(self.class.viewName)
                        .as(self.class.statementSelect),
                    @"CREATE VIEW IF NOT EXISTS testView AS SELECT testColumn FROM testTable");
}

@end

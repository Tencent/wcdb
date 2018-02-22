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

@interface WTCStatementDropViewTests : WTCWINQTestCase

@end

@implementation WTCStatementDropViewTests

- (void)testStatementDropView
{
    WINQAssertEqual(WCDB::StatementDropView()
                        .dropView(self.class.viewName, true)
                        .withSchema(self.class.schemaName),
                    @"DROP VIEW IF EXISTS testSchema.testView");

    WINQAssertEqual(WCDB::StatementDropView()
                        .dropView(self.class.viewName, true),
                    @"DROP VIEW IF EXISTS testView");

    WINQAssertEqual(WCDB::StatementDropView()
                        .dropView(self.class.viewName, false)
                        .withSchema(self.class.schemaName),
                    @"DROP VIEW testSchema.testView");

    //Default
    WINQAssertEqual(WCDB::StatementDropView()
                        .dropView(self.class.viewName)
                        .withSchema(self.class.schemaName),
                    @"DROP VIEW IF EXISTS testSchema.testView");
}

@end

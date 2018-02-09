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

@interface WTCForeignKeyClauseTests : XCTestCase

@end

@implementation WTCForeignKeyClauseTests

- (void)testForeignKeyClause
{
    std::string tableName = "testTable";
    std::string columName1 = "testColumn1";
    std::list<std::string> columNames = {columName1, "testColumn2"};
    std::string name = "matchingName";

    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onDeleteSetNull().notDeferrable().initiallyDeferred(), @"REFERENCES testTable(testColumn1) ON DELETE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");
    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onDeleteSetNull().notDeferrable().initiallyImmediate(), @"REFERENCES testTable(testColumn1) ON DELETE SET NULL NOT DEFERRABLE INITIALLY IMMEDIATE");
    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onDeleteSetNull().notDeferrable(), @"REFERENCES testTable(testColumn1) ON DELETE SET NULL NOT DEFERRABLE");
    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onDeleteSetNull().deferrable().initiallyDeferred(), @"REFERENCES testTable(testColumn1) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED");
    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onDeleteSetNull(), @"REFERENCES testTable(testColumn1) ON DELETE SET NULL");

    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onDeleteSetDefault().notDeferrable().initiallyDeferred(), @"REFERENCES testTable(testColumn1) ON DELETE SET DEFAULT NOT DEFERRABLE INITIALLY DEFERRED");
    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onDeleteCascade().notDeferrable().initiallyDeferred(), @"REFERENCES testTable(testColumn1) ON DELETE CASCADE NOT DEFERRABLE INITIALLY DEFERRED");
    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onDeleteRestrict().notDeferrable().initiallyDeferred(), @"REFERENCES testTable(testColumn1) ON DELETE RESTRICT NOT DEFERRABLE INITIALLY DEFERRED");
    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onDeleteNoAction().notDeferrable().initiallyDeferred(), @"REFERENCES testTable(testColumn1) ON DELETE NO ACTION NOT DEFERRABLE INITIALLY DEFERRED");
    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onUpdateSetNull().notDeferrable().initiallyDeferred(), @"REFERENCES testTable(testColumn1) ON UPDATE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).byMatching(name).notDeferrable().initiallyDeferred(), @"REFERENCES testTable(testColumn1) MATCH matchingName NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnName(columName1).onDeleteSetNull().byMatching(name).notDeferrable().initiallyDeferred(), @"REFERENCES testTable(testColumn1) ON DELETE SET NULL MATCH matchingName NOT DEFERRABLE INITIALLY DEFERRED");
    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).byAddingColumnNames(columNames).onDeleteSetNull().notDeferrable().initiallyDeferred(), @"REFERENCES testTable(testColumn1, testColumn2) ON DELETE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");
    WINQAssertEqual(WCDB::ForeignKeyClause(tableName).onDeleteSetNull().notDeferrable().initiallyDeferred(), @"REFERENCES testTable ON DELETE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");
}

@end

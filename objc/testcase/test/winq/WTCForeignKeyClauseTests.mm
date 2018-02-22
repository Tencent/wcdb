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

@interface WTCForeignKeyClauseTests : WTCWINQTestCase

@end

@implementation WTCForeignKeyClauseTests

- (void)testForeignKeyClause
{
    std::string foreignTableName = "testForeignTable";
    std::string matchName = "testMatchName";

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onDeleteSetNull()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON DELETE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onDeleteSetNull()
                        .notDeferrable()
                        .initiallyImmediate(),
                    @"REFERENCES testForeignTable(testColumn) ON DELETE SET NULL NOT DEFERRABLE INITIALLY IMMEDIATE");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onDeleteSetNull()
                        .notDeferrable(),
                    @"REFERENCES testForeignTable(testColumn) ON DELETE SET NULL NOT DEFERRABLE");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onDeleteSetNull()
                        .deferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onDeleteSetDefault()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON DELETE SET DEFAULT NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onDeleteCascade()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON DELETE CASCADE NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onDeleteRestrict()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON DELETE RESTRICT NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onDeleteNoAction()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON DELETE NO ACTION NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onUpdateSetNull()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON UPDATE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onUpdateSetDefault()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON UPDATE SET DEFAULT NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onUpdateCascade()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON UPDATE CASCADE NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onUpdateRestrict()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON UPDATE RESTRICT NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onUpdateNoAction()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON UPDATE NO ACTION NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .byMatching(matchName)
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) MATCH testMatchName NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .onDeleteSetNull()
                        .byMatching(matchName)
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) ON DELETE SET NULL MATCH testMatchName NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnName(self.class.columnName)
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn) NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .byAddingColumnNames(self.class.columnNames)
                        .onDeleteSetNull()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable(testColumn, testColumn2) ON DELETE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual(WCDB::ForeignKeyClause(foreignTableName)
                        .onDeleteSetNull()
                        .notDeferrable()
                        .initiallyDeferred(),
                    @"REFERENCES testForeignTable ON DELETE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");
}

@end

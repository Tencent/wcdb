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
#import "WTCBaseTestCase.h"

@interface WTCForeignKeyTests : WTCBaseTestCase

@end

@implementation WTCForeignKeyTests

- (WCDB::ForeignKey)generateForeignKey
{
    WCDB::Column column1("column1");
    WCDB::Column column2("column2");
    return WCDB::ForeignKey("testForeignKeyTable", {column1, column2});
}

- (void)testForeignKey
{
    WINQAssertEqual([self generateForeignKey].onDelete(WCDB::ForeignKey::Action::SetNull),
                    @"REFERENCES testForeignKeyTable(column1, column2) ON DELETE SET NULL");

    WINQAssertEqual([self generateForeignKey].onUpdate(WCDB::ForeignKey::Action::SetDefault),
                    @"REFERENCES testForeignKeyTable(column1, column2) ON UPDATE SET DEFAULT");

    WINQAssertEqual([self generateForeignKey].onDelete(WCDB::ForeignKey::Action::Cascade),
                    @"REFERENCES testForeignKeyTable(column1, column2) ON DELETE CASCADE");

    WINQAssertEqual([self generateForeignKey].onUpdate(WCDB::ForeignKey::Action::Restrict),
                    @"REFERENCES testForeignKeyTable(column1, column2) ON UPDATE RESTRICT");

    WINQAssertEqual([self generateForeignKey].onDelete(WCDB::ForeignKey::Action::NoAction),
                    @"REFERENCES testForeignKeyTable(column1, column2) ON DELETE NO ACTION");

    WINQAssertEqual([self generateForeignKey].deferrable(WCDB::ForeignKey::Deferrable::Deferred),
                    @"REFERENCES testForeignKeyTable(column1, column2) DEFERRABLE INITIALLY DEFERRED");

    WINQAssertEqual([self generateForeignKey].notDeferrable(WCDB::ForeignKey::Deferrable::Immediate),
                    @"REFERENCES testForeignKeyTable(column1, column2) NOT DEFERRABLE INITIALLY IMMEDIATE");

    WINQAssertEqual([self generateForeignKey].match("test"),
                    @"REFERENCES testForeignKeyTable(column1, column2) MATCH test");
}

@end

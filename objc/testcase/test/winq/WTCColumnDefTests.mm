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

@interface WTCColumnDefTests : XCTestCase

@end

@implementation WTCColumnDefTests

- (void)testColumnDef
{
    WCDB::ColumnType type = WCDB::ColumnType::Integer32;
    WCDB::ColumnConstraint columnConstraint1 = WCDB::ColumnConstraint().named("testConstraint1").withNotNull();
    WCDB::ColumnConstraint columnConstraint2 = WCDB::ColumnConstraint().named("testConstraint2").withUnique();
    std::list<WCDB::ColumnConstraint> columnConstraintList = {columnConstraint1, columnConstraint2};

    WINQAssertEqual(WCDB::ColumnDef("localID").withType(type), @"localID INTEGER");

    WINQAssertEqual(WCDB::ColumnDef("localID"), @"localID");

    WINQAssertEqual(WCDB::ColumnDef("localID").withType(type).byAddingConstraint(columnConstraint1), @"localID INTEGER CONSTRAINT testConstraint1 NOT NULL");

    WINQAssertEqual(WCDB::ColumnDef("localID").withType(type).byAddingConstraints(columnConstraintList), @"localID INTEGER CONSTRAINT testConstraint1 NOT NULL CONSTRAINT testConstraint2 UNIQUE");
}

@end

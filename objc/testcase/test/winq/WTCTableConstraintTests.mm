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

using namespace WCDB;

@interface WTCTableConstraintTests : WTCBaseTestCase

@end

@implementation WTCTableConstraintTests


- (TableConstraint)generateTableConstraint
{
    return TableConstraint("table1");
}

- (void)testTableConstraint
{
    //Give
    Column column1("column1");
    ColumnIndex index1(column1);
    ColumnIndex index2 = ColumnIndex(Column("column2"));
    ForeignKey foreignKey1("testTable", column1);
    
    WINQAssertEqual([self generateTableConstraint].makePrimary({index1, index2}), @"CONSTRAINT table1 PRIMARY KEY(column1, column2)");
    
    WINQAssertEqual([self generateTableConstraint].makeUnique({index1, index2}), @"CONSTRAINT table1 UNIQUE(column1, column2)");
    
    WINQAssertEqual([self generateTableConstraint].onConflict(Conflict::Fail), @"CONSTRAINT table1 ON CONFLICT FAIL");
    
    WINQAssertEqual([self generateTableConstraint].check(column1 > 1), @"CONSTRAINT table1 CHECK(column1 > 1)");
    
    WINQAssertEqual([self generateTableConstraint].makeForeignKey(column1, foreignKey1), @"CONSTRAINT table1 FOREIGN KEY(column1) REFERENCES testTable(column1)");
}

@end

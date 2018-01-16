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

@interface WTCColumnDefTests : WTCBaseTestCase

@end

@implementation WTCColumnDefTests

- (ColumnDef)generateColumnDef
{
    return ColumnDef(Column("testColumnDef"), ColumnType::Integer32);
}

- (void)testColumnDef
{

    WINQAssertEqual(self.generateColumnDef.makePrimary(), @"testColumnDef INTEGER PRIMARY KEY");

    WINQAssertEqual(self.generateColumnDef.makePrimary(OrderTerm::ASC), @"testColumnDef INTEGER PRIMARY KEY ASC");

    WINQAssertEqual(self.generateColumnDef.makePrimary(OrderTerm::NotSet, true), @"testColumnDef INTEGER PRIMARY KEY AUTOINCREMENT");

    WINQAssertEqual(self.generateColumnDef.makePrimary(OrderTerm::NotSet, false, Conflict::Ignore), @"testColumnDef INTEGER PRIMARY KEY ON CONFLICT IGNORE");

    WINQAssertEqual(self.generateColumnDef.makeDefault(nullptr), @"testColumnDef INTEGER DEFAULT NULL");

    WINQAssertEqual(self.generateColumnDef.makeDefault(-1), @"testColumnDef INTEGER DEFAULT -1");

    WINQAssertEqual(self.generateColumnDef.makeDefault(171798691840), @"testColumnDef INTEGER DEFAULT 171798691840");

    WINQAssertEqual(self.generateColumnDef.makeDefault(false), @"testColumnDef INTEGER DEFAULT 0");
    WINQAssertEqual(self.generateColumnDef.makeDefault("SampleText"), @"testColumnDef INTEGER DEFAULT 'SampleText'");

    WINQAssertEqual(self.generateColumnDef.makeDefault(0.1), @"testColumnDef INTEGER DEFAULT 0.100000");

    WINQAssertEqual(self.generateColumnDef.makeDefault(1), @"testColumnDef INTEGER DEFAULT 1");

    WINQAssertEqual(self.generateColumnDef.makeDefault(ColumnDef::DefaultType::CurrentDate), @"testColumnDef INTEGER DEFAULT CURRENT_DATE");

    WINQAssertEqual(self.generateColumnDef.makeDefault(ColumnDef::DefaultType::CurrentTime), @"testColumnDef INTEGER DEFAULT CURRENT_TIME");

    WINQAssertEqual(self.generateColumnDef.makeDefault(ColumnDef::DefaultType::CurrentTimestamp), @"testColumnDef INTEGER DEFAULT CURRENT_TIMESTAMP");

    WINQAssertEqual(self.generateColumnDef.makeNotNull(), @"testColumnDef INTEGER NOT NULL");

    WINQAssertEqual(self.generateColumnDef.makeUnique(), @"testColumnDef INTEGER UNIQUE");

    WINQAssertEqual(self.generateColumnDef.makeForeignKey(ForeignKey("testColumnDefTable")), @"testColumnDef INTEGER REFERENCES testColumnDefTable");
}

@end

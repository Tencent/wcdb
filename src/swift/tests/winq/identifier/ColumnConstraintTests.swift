//
// Created by qiuwenchen on 2022/5/29.
//

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

import Foundation
#if TEST_WCDB_SWIFT
import WCDBSwift
#else
import WCDB
#endif

class ColumnConstraintTests: BaseTestCase {

    func testColumnConstraint() {
        WINQAssertEqual(ColumnConstraint(), "")

        let name = "testColumnConstraint"

        WINQAssertEqual(ColumnConstraint().primaryKey(), "PRIMARY KEY")

        WINQAssertEqual(ColumnConstraint(named: name).primaryKey(), "CONSTRAINT \(name) PRIMARY KEY")

        WINQAssertEqual(ColumnConstraint().primaryKey().order(.ascending), "PRIMARY KEY ASC")

        WINQAssertEqual(ColumnConstraint().primaryKey().conflict(action: .Abort), "PRIMARY KEY ON CONFLICT ABORT")

        WINQAssertEqual(ColumnConstraint().primaryKey().autoIncrement(), "PRIMARY KEY AUTOINCREMENT")

        WINQAssertEqual(ColumnConstraint(named: name).notNull(), "CONSTRAINT \(name) NOT NULL")

        WINQAssertEqual(ColumnConstraint().notNull().conflict(action: .Abort), "NOT NULL ON CONFLICT ABORT")

        WINQAssertEqual(ColumnConstraint(named: name).unique(), "CONSTRAINT \(name) UNIQUE")

        WINQAssertEqual(ColumnConstraint(named: name).unique().conflict(action: .Abort), "CONSTRAINT \(name) UNIQUE ON CONFLICT ABORT")

        WINQAssertEqual(ColumnConstraint(named: name).unique().collate(with: "BINARY"), "CONSTRAINT \(name) COLLATE BINARY")

    }
}

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
import WCDB_Private

public final class Column: Identifier<CPPColumn> {

    public static func all() -> Column {
        Column(with: WCDBColumnCreateAll())
    }

    public static func rowid() -> Column {
        Column(with: WCDBColumnCreateRowId())
    }

    public convenience init(named name: String) {
        self.init(named: name, binded: nil)
    }

    internal convenience init(named name: String, binded tableBinding: UnsafeRawPointer?) {
        self.init(with: WCDBColumnCreateWithName(name.cString, tableBinding))
    }

    internal override init(with cppObj: CPPColumn) {
        super.init(with: cppObj)
    }

    @discardableResult
    public func `in`(table: String) -> Column {
        WCDBColumnInTable(cppObj, table.cString)
        return self
    }

    public func `as`(_ alias: String) -> ResultColumn {
        return ResultColumn(with: WCDBColumnConfigAlias(cppObj, alias.cString))
    }

    @discardableResult
    public func of(schema schemaConvertible: SchemaConvertible) -> Column {
        let schema = schemaConvertible.asSchema()
        withExtendedLifetime(schema) {
            WCDBColumnOfSchema(cppObj, schema.cppObj)
        }
        return self
    }
}

extension Column: ColumnConvertible, ExpressionOperable {
    public func asColumn() -> Column {
        return self
    }
}

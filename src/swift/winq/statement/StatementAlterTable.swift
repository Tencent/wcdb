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

public final class StatementAlterTable: Identifier<CPPStatementAlterTable>, Statement {
    public init() {
        super.init(with: WCDBStatementAlterTableCreate())
    }

    @discardableResult
    public func alter(table: String) -> StatementAlterTable {
        WCDBStatementAlterTableConfigTable(cppObj, table.cString)
        return self
    }

    @discardableResult
    public func schema(_ schemaConvertible: SchemaConvertible) -> StatementAlterTable {
        let schema = schemaConvertible.asSchema()
        withExtendedLifetime(schema) {
            WCDBStatementAlterTableConfigSchema(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func rename(to newTable: String) -> StatementAlterTable {
        WCDBStatementAlterTableConfigRenameToTable(cppObj, newTable.cString)
        return self
    }

    public func rename(column columnConvertible: ColumnConvertible, to newColumnConvertible: ColumnConvertible) -> StatementAlterTable {
        let oldColumn = columnConvertible.asColumn()
        let newColumn = newColumnConvertible.asColumn()
        ObjectBridge.extendLifetime(oldColumn, newColumn) {
            WCDBStatementAlterTableConfigRenameColumn(self.cppObj, oldColumn.cppObj)
            WCDBStatementAlterTableConfigRenameToColumn(self.cppObj, newColumn.cppObj)
        }
        return self
    }

    @discardableResult
    public func addColumn(with columnDef: ColumnDef) -> StatementAlterTable {
        withExtendedLifetime(columnDef) {
            WCDBStatementAlterTableConfigAddColumn(cppObj, columnDef.cppObj)
        }
        return self
    }
}

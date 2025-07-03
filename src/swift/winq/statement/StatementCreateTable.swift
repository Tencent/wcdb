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
public final class StatementCreateTable: Identifier<CPPStatementCreateTable>, Statement {
    public init() {
        super.init(with: WCDBStatementCreateTableCreate())
    }

    @discardableResult
    public func create(table: String, isTemp: Bool = false) -> StatementCreateTable {
        WCDBStatementCreateTableConfigTable(cppObj, table.cString)
        if  isTemp {
            WCDBStatementCreateTableConfigTemp(cppObj)
        }
        return self
    }

    @discardableResult
    public func of(schema schemaConvertible: SchemaConvertible) -> StatementCreateTable {
        let schema = schemaConvertible.asSchema()
        withExtendedLifetime(schema) {
            WCDBStatementCreateTableConfigSchema(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func ifNotExists() -> StatementCreateTable {
        WCDBStatementCreateTableConfigIfNotExist(cppObj)
        return self
    }

    @discardableResult
    public func `as`(_ select: StatementSelect) -> StatementCreateTable {
        withExtendedLifetime(select) {
            WCDBStatementCreateTableConfigAs(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func with(columns: ColumnDef...) -> StatementCreateTable {
        with(columns: columns)
    }

    @discardableResult
    public func with(columns: [ColumnDef]) -> StatementCreateTable {
        withExtendedLifetime(columns) {
            let cppColumns = $0.map { $0.cppObj }
            cppColumns.withUnsafeBufferPointer {
                WCDBStatementCreateTableConfigColumns(cppObj, $0.baseAddress, Int32($0.count) )
            }
        }
        return self
    }

    @discardableResult
    public func constraint(_ constraints: TableConstraint...) -> StatementCreateTable {
        constraint(constraints)
    }

    @discardableResult
    public func constraint(_ constraints: [TableConstraint]? = nil) -> StatementCreateTable {
        if let constraints = constraints {
            withExtendedLifetime(constraints) {
                let cppConstrain = $0.map { $0.cppObj }
                cppConstrain.withUnsafeBufferPointer {
                    WCDBStatementCreateTableConfigTableConstraints(cppObj, $0.baseAddress, Int32($0.count) )
                }
            }
        }
        return self
    }

    @discardableResult
    public func withoutRowid() -> StatementCreateTable {
        WCDBStatementCreateTableConfigWithoutRowId(cppObj)
        return self
    }

}

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

public final class StatementCreateIndex: Identifier<CPPStatementCreateIndex>, Statement {
    public init() {
        super.init(with: WCDBStatementCreatIndexCreate())
    }

    @discardableResult
    public func create(index: String, isUnique: Bool = false) -> StatementCreateIndex {
        WCDBStatementCreatIndexConfigIndexName(cppObj, index.cString)
        if isUnique {
            WCDBStatementCreatIndexConfigUniqe(cppObj)
        }
        return self
    }

    @discardableResult
    public func unique() -> StatementCreateIndex {
        WCDBStatementCreatIndexConfigUniqe(cppObj)
        return self
    }

    @discardableResult
    public func ifNotExists() -> StatementCreateIndex {
        WCDBStatementCreatIndexConfigIfNotExist(cppObj)
        return self
    }

    @discardableResult
    public func of(schema schemaConvertible: SchemaConvertible) -> StatementCreateIndex {
        let schema = schemaConvertible.asSchema()
        withExtendedLifetime(schema) {
            WCDBStatementCreatIndexConfigSchema(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func on(table: String) -> StatementCreateIndex {
        WCDBStatementCreatIndexConfigTable(cppObj, table.cString)
        return self
    }

    @discardableResult
    public func indexesBy(_ indexedColumnConvertibleList: IndexedColumnConvertible...) -> StatementCreateIndex {
        return indexesBy(indexedColumnConvertibleList)
    }

    @discardableResult
    public func indexesBy(_ indexedColumnConvertibleList: [IndexedColumnConvertible]) -> StatementCreateIndex {
        let indexedColumns = indexedColumnConvertibleList.map { $0.asIndex() }
        withExtendedLifetime(indexedColumns) {
            let cppIndexedColumns = $0.map { $0.cppObj }
            cppIndexedColumns.withUnsafeBufferPointer {
                WCDBStatementCreatIndexConfigIndexColumns(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func `where`(_ expressionConvertible: ExpressionConvertible) -> StatementCreateIndex {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBStatementCreatIndexConfigWhere(cppObj, $0.cppObj)
        }
        return self
    }
}

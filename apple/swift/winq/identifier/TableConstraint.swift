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
public final class TableConstraint: Identifier<CPPTableConstraint> {
    public init(named name: String? = nil) {
        super.init(with: WCDBTableConstraintCreate(name?.cString))
    }

    @discardableResult
    public func primaryKey() -> TableConstraint {
        WCDBTableConstraintConfigPrimaryKey(cppObj)
        return self
    }

    @discardableResult
    public func unique() -> TableConstraint {
        WCDBTableConstraintConfigUnique(cppObj)
        return self
    }

    @discardableResult
    public func indexed(by indexedColumnConvertibleList: IndexedColumnConvertible...) -> TableConstraint {
        indexed(by: indexedColumnConvertibleList)
    }

    @discardableResult
    public func indexed(by indexedColumnConvertibleList: [IndexedColumnConvertible]) -> TableConstraint {
        let indexedColumns = indexedColumnConvertibleList.map { $0.asIndex() }
        withExtendedLifetime(indexedColumns) {
            let cppIndexedColumn = indexedColumns.map { $0.cppObj }
            cppIndexedColumn.withUnsafeBufferPointer {
                WCDBTableConstraintConfigIndexedColumn(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func onConflict(_ conflict: ConflictAction) -> TableConstraint {
        WCDBTableConstraintConfigConfliction(cppObj, conflict.cValue)
        return self
    }

    @discardableResult
    public func check(_ expressionConvertible: ExpressionConvertible) -> TableConstraint {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBTableConstraintConfigCheckCondition(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func foreignKey(_ columnConvertibleList: ColumnConvertible...,
                               foreignKey: ForeignKey) -> TableConstraint {
        return self.foreignKey(columnConvertibleList, foreignKey: foreignKey)
    }

    @discardableResult
    public func foreignKey(_ columnConvertibleList: [ColumnConvertible],
                               foreignKey: ForeignKey) -> TableConstraint {
        let columns = columnConvertibleList.map { $0.asColumn() }
        ObjectBridge.extendLifetime(columns, foreignKey) {
            let cppColumns = columns.map { $0.cppObj }
            cppColumns.withUnsafeBufferPointer {
                WCDBTableConstraintConfigForeignKey(self.cppObj, $0.baseAddress, Int32($0.count), foreignKey.cppObj)
            }
        }
        return self
    }
}

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

public final class ColumnDef: Identifier<CPPColumnDef> {

    convenience public init(with columnConvertible: ColumnConvertible, and optionalType: ColumnType? = nil) {
        let column = columnConvertible.asColumn()
        if let type = optionalType {
            let cppColumnDef = withExtendedLifetime(column) {
                WCDBColumnDefCreateWithType(column.cppObj, type.cValue)
            }
            self.init(with: cppColumnDef)
        } else {
            let cppColumnDef = withExtendedLifetime(column) {
                WCDBColumnDefCreateWithoutType(column.cppObj)
            }
            self.init(with: cppColumnDef)
        }
    }

    convenience internal init(named name: String, and optionalType: ColumnType? = nil) {
        let column = Column(named: name)
        self.init(with: column, and: optionalType)
    }

    public func addConstraint(_ constraint: ColumnConstraint) {
        withExtendedLifetime(constraint) {
            WCDBColumnDefConfigConstraint(cppObj, constraint.cppObj)
        }
    }

    @discardableResult
    public func makePrimary(orderBy order: Order? = nil,
                            isAutoIncrement: Bool? = false,
                            onConflict conflict: ConflictAction? = nil) -> ColumnDef {
        let constrant = ColumnConstraint()
        constrant.primaryKey()
        if let order = order {
            constrant.order(order)
        }
        if isAutoIncrement != nil && isAutoIncrement! {
            constrant.autoIncrement()
        }
        if let conflict = conflict {
            constrant.conflict(action: conflict)
        }
        self.addConstraint(constrant)
        return self
    }

    @discardableResult
    public func makeDefault(to defaultValue: ExpressionConvertible) -> ColumnDef {
        self.addConstraint(ColumnConstraint().default(with: defaultValue.asExpression()))
        return self
    }

    @discardableResult
    public func makeNotNull() -> ColumnDef {
        self.addConstraint(ColumnConstraint().notNull())
        return self
    }

    @discardableResult
    public func makeUnique() -> ColumnDef {
        self.addConstraint(ColumnConstraint().unique())
        return self
    }

    @discardableResult
    public func makeForeignKey(_ foreignKey: ForeignKey) -> ColumnDef {
        self.addConstraint(ColumnConstraint().foreignKey(with: foreignKey))
        return self
    }

    @discardableResult
    public func makeNotIndexed() -> ColumnDef {
        self.addConstraint(ColumnConstraint().unIndexed())
        return self
    }
}

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

public protocol TableConstraintBinding {
    typealias Name = String
    func generateConstraint(withName name: String) -> TableConstraint
}

public struct MultiPrimaryBinding: TableConstraintBinding {
    let conflict: Conflict?
    let columnIndexConvertibleList: [ColumnIndexConvertible]

    public init(indexesBy columnIndexConvertibleList: ColumnIndexConvertible...,
                onConflict conflict: Conflict? = nil) {
        self.init(indexesBy: columnIndexConvertibleList, onConflict: conflict)
    }

    public init(indexesBy columnIndexConvertibleList: [ColumnIndexConvertible],
                onConflict conflict: Conflict? = nil) {
        self.columnIndexConvertibleList = columnIndexConvertibleList
        self.conflict = conflict
    }

    public func generateConstraint(withName name: String) -> TableConstraint {
        let tableConstraint = TableConstraint(named: name).makePrimary(indexesBy: columnIndexConvertibleList)
        if let wrappedConflict = conflict {
            tableConstraint.onConflict(wrappedConflict)
        }
        return tableConstraint
    }
}

public struct MultiUniqueBinding: TableConstraintBinding {
    let conflict: Conflict?
    let columnIndexConvertibleList: [ColumnIndexConvertible]

    public init(indexesBy columnIndexConvertibleList: ColumnIndexConvertible...,
                onConflict conflict: Conflict? = nil) {
        self.init(indexesBy: columnIndexConvertibleList, onConflict: conflict)
    }

    public init(indexesBy columnIndexConvertibleList: [ColumnIndexConvertible],
                onConflict conflict: Conflict? = nil) {
        self.columnIndexConvertibleList = columnIndexConvertibleList
        self.conflict = conflict
    }

    public func generateConstraint(withName name: String) -> TableConstraint {
        let tableConstraint = TableConstraint(named: name).makeUnique(indexesBy: columnIndexConvertibleList)
        if let wrappedConflict = conflict {
            tableConstraint.onConflict(wrappedConflict)
        }
        return tableConstraint
    }
}

public struct CheckBinding: TableConstraintBinding {
    let condition: Expression

    public init(check condition: Expression) {
        self.condition = condition
    }

    public func generateConstraint(withName name: String) -> TableConstraint {
        return TableConstraint(named: name).check(condition)
    }
}

public struct ForeignKeyBinding: TableConstraintBinding {
    let columnConvertibleList: [ColumnConvertible]
    let foreignKey: ForeignKey

    public init(_ columnConvertibleList: ColumnConvertible..., foreignKey: ForeignKey) {
        self.init(columnConvertibleList, foreignKey: foreignKey)
    }

    public init(_ columnConvertibleList: [ColumnConvertible], foreignKey: ForeignKey) {
        self.columnConvertibleList = columnConvertibleList
        self.foreignKey = foreignKey
    }

    public func generateConstraint(withName name: String) -> TableConstraint {
        return TableConstraint(named: name).makeForeignKey(columnConvertibleList, foreignKey: foreignKey)
    }
}

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
public final class TableConstraint: Describable {
    public private(set) var description: String

    public init(named name: String) {
        description = "CONSTRAINT \(name)"
    }

    @discardableResult
    public func makePrimary(indexesBy columnIndexConvertibleList: ColumnIndexConvertible...) -> TableConstraint {
        return makePrimary(indexesBy: columnIndexConvertibleList)
    }

    @discardableResult
    public func makePrimary(indexesBy columnIndexConvertibleList: [ColumnIndexConvertible]) -> TableConstraint {
        description.append(" PRIMARY KEY(\(columnIndexConvertibleList.joined()))")
        return self
    }

    @discardableResult
    public func makeUnique(indexesBy columnIndexConvertibleList: ColumnIndexConvertible...) -> TableConstraint {
        return makeUnique(indexesBy: columnIndexConvertibleList)
    }

    @discardableResult
    public func makeUnique(indexesBy columnIndexConvertibleList: [ColumnIndexConvertible]) -> TableConstraint {
        description.append(" UNIQUE(\(columnIndexConvertibleList.joined()))")
        return self
    }

    @discardableResult
    public func onConflict(_ conflict: Conflict) -> TableConstraint {
        description.append(" ON CONFLICT \(conflict.description)")
        return self
    }

    @discardableResult
    public func check(_ expressionConvertible: ExpressionConvertible) -> TableConstraint {
        description.append(" CHECK\(expressionConvertible.asExpression().description)")
        return self
    }

    @discardableResult
    public func makeForeignKey(_ columnConvertibleList: ColumnConvertible...,
                               foreignKey: ForeignKey) -> TableConstraint {
        return makeForeignKey(columnConvertibleList, foreignKey: foreignKey)
    }

    @discardableResult
    public func makeForeignKey(_ columnConvertibleList: [ColumnConvertible],
                               foreignKey: ForeignKey) -> TableConstraint {
        description.append(" FOREIGN KEY(\(columnConvertibleList.joined())) \(foreignKey.description)")
        return self
    }
}

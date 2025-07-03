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
import WCDB_Private

public final class ColumnConstraint: Identifier<CPPColumnConstraint> {

    public convenience init(named name: String? = nil) {
        self.init(with: WCDBColumnConstraintCreate(name?.cString))
    }

    @discardableResult
    public func primaryKey() -> ColumnConstraint {
        WCDBColumnConstraintConfigPrimaryKey(cppObj)
        return self
    }

    @discardableResult
    public func order(_ order: Order) -> ColumnConstraint {
        WCDBColumnConstraintConfigOrder(cppObj, order.cValue)
        return self
    }

    @discardableResult
    public func conflict(action: ConflictAction) -> ColumnConstraint {
        WCDBColumnConstraintConfigCoflictAction(cppObj, action.cValue)
        return self
    }

    @discardableResult
    public func autoIncrement() -> ColumnConstraint {
        WCDBColumnConstraintConfigAutoIncrement(cppObj)
        return self
    }

    @discardableResult
    public func notNull() -> ColumnConstraint {
        WCDBColumnConstraintConfigNotNull(cppObj)
        return self
    }

    @discardableResult
    public func unique() -> ColumnConstraint {
        WCDBColumnConstraintConfigUnique(cppObj)
        return self
    }

    @discardableResult
    public func check(with expression: Expression) -> ColumnConstraint {
        withExtendedLifetime(expression) {
            WCDBColumnConstraintConfigCheck(cppObj, expression.cppObj)
        }
        return self
    }

    @discardableResult
    public func `default`(with expressionConvertible: ExpressionConvertible) -> ColumnConstraint {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBColumnConstraintConfigDefaultValue(cppObj, expression.cppObj)
        }
        return self
    }

    @discardableResult
    public func collate(with collation: String) -> ColumnConstraint {
        WCDBColumnConstraintConfigCollation(cppObj, collation.cString)
        return self
    }

    @discardableResult
    public func foreignKey(with key: ForeignKey) -> ColumnConstraint {
        withExtendedLifetime(key) {
            WCDBColumnConstraintConfigForeignKey(cppObj, key.cppObj)
        }
        return self
    }

    @discardableResult
    public func unIndexed() -> ColumnConstraint {
        WCDBColumnConstraintConfigUnIndexed(cppObj)
        return self
    }
}

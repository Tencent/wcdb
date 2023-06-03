//
// Created by qiuwenchen on 2022/6/12.
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

public final class Upsert: Identifier<CPPUpsert> {

    public init() {
        super.init(with: WCDBUpsertCreate())
    }

    @discardableResult
    public func onConflict() -> Upsert {
        return self
    }

    @discardableResult
    public func indexed(by indexedColumnConvertibleList: IndexedColumnConvertible...) -> Upsert {
        indexed(by: indexedColumnConvertibleList)
    }

    @discardableResult
    public func indexed(by indexedColumnConvertibleList: [IndexedColumnConvertible]) -> Upsert {
        let indexedColumns = indexedColumnConvertibleList.map { $0.asIndex() }
        withExtendedLifetime(indexedColumns) {
            let cppIndexedColumn = indexedColumns.map { $0.cppObj }
            cppIndexedColumn.withUnsafeBufferPointer {
                WCDBUpsertConfigIndexdColumn(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func `where`(_ expressionConvertible: ExpressionConvertible) -> Upsert {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBUpsertConfigWhere(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func doNothing() -> Upsert {
        WCDBUpsertConfigDoNothing(cppObj)
        return self
    }

    @discardableResult
    public func doUpdate() -> Upsert {
        WCDBUpsertConfigDoUpdate(cppObj)
        return self
    }

    @discardableResult
    public func set(_ columnConvertible: ColumnConvertible...) -> Upsert {
        set(columnConvertible)
    }

    @discardableResult
    public func set(_ columnConvertible: [ColumnConvertible]) -> Upsert {
        let columns = columnConvertible.map { $0.asColumn() }
        withExtendedLifetime(columns) {
            let cppColumns = $0.map { $0.cppObj }
            cppColumns.withUnsafeBufferPointer {
                WCDBUpsertConfigSetColumns(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func to(_ expressionConvertible: ExpressionConvertible) -> Upsert {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBUpsertConfigToValue(cppObj, $0.cppObj)
        }
        return self
    }
}

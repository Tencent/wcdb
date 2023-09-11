//
// Created by qiuwenchen on 2022/7/14.
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

public final class StatementCreateTrigger: Identifier<CPPStatementCreateTrigger>, Statement {
    public init() {
        super.init(with: WCDBStatementCreateTriggerCreate())
    }

    @discardableResult
    public func create(trigger: String, isTemp: Bool = false) -> StatementCreateTrigger {
        WCDBStatementCreateTriggerConfigTrigger(cppObj, trigger.cString)
        if  isTemp {
            WCDBStatementCreateTriggerConfigTemp(cppObj)
        }
        return self
    }

    @discardableResult
    public func of(schema schemaConvertible: SchemaConvertible) -> StatementCreateTrigger {
        let schema = schemaConvertible.asSchema()
        withExtendedLifetime(schema) {
            WCDBStatementCreateTriggerConfigSchema(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func ifNotExists() -> StatementCreateTrigger {
        WCDBStatementCreateTriggerConfigIfNotExist(cppObj)
        return self
    }

    @discardableResult
    public func before() -> StatementCreateTrigger {
        WCDBStatementCreateTriggerConfigBefore(cppObj)
        return self
    }

    @discardableResult
    public func after() -> StatementCreateTrigger {
        WCDBStatementCreateTriggerConfigAfter(cppObj)
        return self
    }

    @discardableResult
    public func insteadOf() -> StatementCreateTrigger {
        WCDBStatementCreateTriggerConfigInsteadOf(cppObj)
        return self
    }

    @discardableResult
    public func delete() -> StatementCreateTrigger {
        WCDBStatementCreateTriggerConfigDelete(cppObj)
        return self
    }

    @discardableResult
    public func insert() -> StatementCreateTrigger {
        WCDBStatementCreateTriggerConfigInsert(cppObj)
        return self
    }

    @discardableResult
    public func update() -> StatementCreateTrigger {
        WCDBStatementCreateTriggerConfigUpdate(cppObj)
        return self
    }

    @discardableResult
    public func of(columns columnConvertibleList: ColumnConvertible...) -> StatementCreateTrigger {
        of(columns: columnConvertibleList)
    }

    @discardableResult
    public func of(columns columnConvertibleList: [ColumnConvertible]) -> StatementCreateTrigger {
        let columns = columnConvertibleList.map {$0.asColumn()}
        withExtendedLifetime(columns) {
            let cppColumns = columns.map {$0.cppObj}
            cppColumns.withUnsafeBufferPointer({
                WCDBStatementCreateTriggerConfigColumns(cppObj, $0.baseAddress, Int32($0.count))
            })
        }
        return self
    }

    @discardableResult
    public func on(table: String) -> StatementCreateTrigger {
        WCDBStatementCreateTriggerConfigTable(cppObj, table.cString)
        return self
    }

    @discardableResult
    public func forEachRow() -> StatementCreateTrigger {
        WCDBStatementCreateTriggerConfigForEachRow(cppObj)
        return self
    }

    @discardableResult
    public func when(_ expressionConvertible: ExpressionConvertible) -> StatementCreateTrigger {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBStatementCreateTriggerConfigWhen(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func execute(_ insert: StatementInsert) -> StatementCreateTrigger {
        withExtendedLifetime(insert) {
            WCDBStatementCreateTriggerExecuteInsert(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func execute(_ update: StatementUpdate) -> StatementCreateTrigger {
        withExtendedLifetime(update) {
            WCDBStatementCreateTriggerExecuteUpdate(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func execute(_ delete: StatementDelete) -> StatementCreateTrigger {
        withExtendedLifetime(delete) {
            WCDBStatementCreateTriggerExecuteDelete(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func execute(_ select: StatementSelect) -> StatementCreateTrigger {
        withExtendedLifetime(select) {
            WCDBStatementCreateTriggerExecuteSelect(cppObj, $0.cppObj)
        }
        return self
    }

}

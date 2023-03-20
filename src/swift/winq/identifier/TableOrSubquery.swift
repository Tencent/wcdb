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
public final class TableOrSubquery: Identifier<CPPTableOrSubquery> {

    public convenience init(withTable table: String) {
        self.init(with: WCDBTableOrSubqueryCreateWithTable(table.cString))
    }

    public convenience init(with join: Join) {
        let cppSubquery = withExtendedLifetime(join) {
            WCDBTableOrSubqueryCreateWithJoin($0.cppObj)
        }
        self.init(with: cppSubquery)
    }

    public convenience init(with tableOrSubqueryConvertibleList: TableOrSubqueryConvertible...) {
        self.init(with: tableOrSubqueryConvertibleList)
    }

    public convenience init(with tableOrSubqueryConvertibleList: [TableOrSubqueryConvertible]) {
        let queries = tableOrSubqueryConvertibleList.map { $0.asTableOrSubquery() }
        let cppTableOrSubquery: CPPTableOrSubquery = withExtendedLifetime(queries) {
            let cppQuries = $0.map { $0.cppObj }
            return cppQuries.withUnsafeBufferPointer {
                return WCDBTableOrSubqueryCreateWithTableOrSubqueries($0.baseAddress, Int32($0.count))
            }
        }
        self.init(with: cppTableOrSubquery)
    }

    public convenience init(with statementSelect: StatementSelect) {
        let cppSubquery = withExtendedLifetime(statementSelect) {
            WCDBTableOrSubqueryCreateWithSelection($0.cppObj)
        }
        self.init(with: cppSubquery)
    }

    @discardableResult
    public func `as`(alias: String) -> TableOrSubquery {
        WCDBTableOrSubqueryConfigAs(cppObj, alias.cString)
        return self
    }

    @discardableResult
    public func of(schema schemaConvertible: SchemaConvertible) -> TableOrSubquery {
        let schema = schemaConvertible.asSchema()
        withExtendedLifetime(schema) {
            WCDBTableOrSubqueryConfigSchema(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func notIndexed() -> TableOrSubquery {
        WCDBTableOrSubqueryConfigNotIndexed(cppObj)
        return self
    }

    @discardableResult
    public func indexed(by indexName: String) -> TableOrSubquery {
        WCDBTableOrSubqueryConfigIndexName(cppObj, indexName.cString)
        return self
    }

    public static func function(_ name: String) -> TableOrSubquery {
        return TableOrSubquery(with: WCDBTableOrSubqueryCreateWithFunction(name.cString))
    }

    @discardableResult
    public func arguments(_ expressionConvertibleList: [ExpressionConvertible]) -> TableOrSubquery {
        let expressions = expressionConvertibleList.map { $0.asExpression() }
        withExtendedLifetime(expressions) {
            let arguments = $0.map { $0.cppObj }
            arguments.withUnsafeBufferPointer { cppArgs in
                WCDBTableOrSubqueryConfigArguments(cppObj, cppArgs.baseAddress, Int32(cppArgs.count))
            }
        }
        return self
    }

    @discardableResult
    public func arguments(_ expressionConvertibleList: ExpressionConvertible...) -> TableOrSubquery {
        return self.arguments(expressionConvertibleList)
    }

    public static func master() -> TableOrSubquery {
        return TableOrSubquery(withTable: "sqlite_master")
    }
}

extension TableOrSubquery: TableOrSubqueryConvertible {
    public func asTableOrSubquery() -> TableOrSubquery {
        return self
    }
}

extension String: TableOrSubqueryConvertible {
    public func asTableOrSubquery() -> TableOrSubquery {
        return TableOrSubquery(withTable: self)
    }
}

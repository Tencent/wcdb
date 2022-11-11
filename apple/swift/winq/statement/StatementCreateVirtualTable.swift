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
public final class StatementCreateVirtualTable: Identifier<CPPStatementCreateVirtualTable>, Statement {
    public init() {
        super.init(with: WCDBStatementCreateVirtualTableCreate())
    }

    @discardableResult
    public func create(virtualTable table: String) -> StatementCreateVirtualTable {
        WCDBStatementCreateVirtualTableConfigTable(cppObj, table.cString)
        return self
    }

    @discardableResult
    public func of(schema schemaConvertible: SchemaConvertible) -> StatementCreateVirtualTable {
        let schema = schemaConvertible.asSchema()
        withExtendedLifetime(schema) {
            WCDBStatementCreateVirtualTableConfigSchema(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func ifNotExists() -> StatementCreateVirtualTable {
        WCDBStatementCreateVirtualTableConfigIfNotExist(cppObj)
        return self
    }

    @discardableResult
    public func using(module: FTSVersion) -> StatementCreateVirtualTable {
        WCDBStatementCreateVirtualTableConfigModule(cppObj, module.description.cString)
        return self
    }

    @discardableResult
    public func using(module: String) -> StatementCreateVirtualTable {
        WCDBStatementCreateVirtualTableConfigModule(cppObj, module.cString)
        return self
    }

    @discardableResult
    public func arguments(_ args: [Describable]) -> StatementCreateVirtualTable {
        var discriptions: [String]
        if args.count == 1, let args = args[0] as? [Describable] {
            discriptions = args.map {
                $0.description
            }
        } else {
            discriptions = args.map {
                $0.description
            }
        }

        withExtendedLifetime(discriptions) {
            let cppArgs = $0.map { $0.cString }
            cppArgs.withUnsafeBufferPointer {
                WCDBStatementCreateVirtualTableConfigArguments(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func arguments(_ args: Describable...) -> StatementCreateVirtualTable {
        arguments(args)
    }
}

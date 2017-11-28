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

public class Delete: CoreRepresentable {
    var core: Core
    private let statement = StatementDelete()
    public var changes: Int = 0

    init(with core: Core, andTableName tableName: String) throws {
        guard tableName.count > 0 else{
            throw Error.reportInterface(tag: core.tag, path: core.path, operation: .Delete, code: .Misuse, message: "Nil table name")
        }
        statement.delete(fromTable: tableName)
        self.core = core
    }    
    
    @discardableResult
    public func `where`(_ condition: Condition) -> Delete {
        statement.where(condition)
        return self
    }

    @discardableResult
    public func order(by orderList: OrderBy...) -> Delete {
        return order(by: orderList)
    }
    
    @discardableResult
    public func order(by orderList: [OrderBy]) -> Delete {
        statement.order(by: orderList)
        return self
    }
    
    @discardableResult
    public func limit(from: Limit, to: Limit) -> Delete {
        statement.limit(from: from, to: to)
        return self
    }
    
    @discardableResult
    public func limit(_ limit: Limit) -> Delete {
        statement.limit(limit)
        return self
    }
    
    @discardableResult
    public func limit(_ limit: Limit, offset: Offset) -> Delete {
        statement.limit(limit, offset: offset)
        return self
    }
    
    public func execute() throws {
        let coreStatement = try core.prepare(statement)
        try coreStatement.step()
        changes = coreStatement.changes
    }
}

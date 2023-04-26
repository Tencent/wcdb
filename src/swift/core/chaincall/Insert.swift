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

/// Chain call for inserting
public final class Insert {
    internal let handle: Handle
    internal var properties: [PropertyConvertible]?
    internal let name: String
    internal var conflict: ConflictAction?

    init(with handle: Handle,
         named name: String,
         on propertyConvertibleList: [PropertyConvertible]?,
         onConflict action: ConflictAction? = nil) {
        self.name = name
        self.properties = propertyConvertibleList
        self.conflict = action
        self.handle = handle
    }

    public final lazy var statement: StatementInsert = StatementInsert()
        .insert(intoTable: name).columns(properties!).onConflict(self.conflict)
        .values(BindParameter.bindParameters(properties!.count))

    /// Execute the insert chain call with objects.
    ///
    /// Note that it will run embedded transaction while objects.count>1.  
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameter objects: Object to be inserted
    /// - Throws: Error
    public func execute<Object: TableEncodable>(with objects: Object...) throws {
        try execute(with: objects)
    }

    /// Execute the insert chain call with objects.
    ///
    /// Note that it will run embedded transaction while objects.count>1.  
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameter objects: Object to be inserted
    /// - Throws: Error
    public func execute<Object: TableEncodable>(with objects: [Object]) throws {
        guard objects.count > 0 else {
            return
        }
        let orm = Object.CodingKeys.objectRelationalMapping
        func doInsertObject(_ handle: Handle) throws {
            properties = properties ?? Object.Properties.all
            try handle.prepare(statement)
            let encoder = TableEncoder(properties!.asCodingTableKeys(), on: handle)
            let useConfliction = conflict != nil
            if !useConfliction {
                encoder.primaryKeyHash = orm.getPrimaryKey()?.stringValue.hashValue
            }

            for var object in objects {
                let isAutoIncrement = object.isAutoIncrement
                encoder.isPrimaryKeyEncoded = !isAutoIncrement
                try object.encode(to: encoder)
                try handle.step()
                assert(!useConfliction || !isAutoIncrement,
                       "Auto-increment inserts do not support conflict action!")
                if !useConfliction && isAutoIncrement {
                    object.lastInsertedRowID = handle.lastInsertedRowID
                }
                handle.reset()
            }
            handle.finalize()
        }
        return objects.count == 1 ? try doInsertObject(handle) : try handle.run(transaction: doInsertObject)
    }
}

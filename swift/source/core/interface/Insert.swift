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
public final class Insert: CoreRepresentable {
    let core: Core
    private var properties: [PropertyConvertible]?
    private let name: String
    private let isReplace: Bool
    public var tag: Tag? {
        return core.tag
    }
    public var path: String {
        return core.path
    }

    init(with core: Core,
         named name: String,
         on propertyConvertibleList: [PropertyConvertible]?,
         isReplace: Bool = false) throws {
        guard name.count > 0 else {
            throw Error.reportInterface(tag: core.tag,
                                        path: core.path,
                                        operation: .insert,
                                        code: .misuse,
                                        message: "Empty table name")
        }
        self.name = name
        self.properties = propertyConvertibleList
        self.isReplace = isReplace
        self.core = core
    }

    private lazy var statement: StatementInsert = StatementInsert()
        .insert(intoTable: name,
                with: properties!,
                onConflict: isReplace ? Conflict.replace : nil)
        .values(Array(repeating: Expression.bindingParameter, count: properties!.count))

    public func execute<Object: TableEncodable>(with objects: Object...) throws {
        try execute(with: objects)
    }

    public func execute<Object: TableEncodable>(with objects: [Object]) throws {
        guard objects.count > 0 else {
            Error.warning("Inserting with an empty/nil object")
            return
        }
        let orm = Object.CodingKeys.objectRelationalMapping
        func doInsertObject() throws {
            properties = properties ?? Object.Properties.all
            let coreStatement = try core.prepare(statement)
            let encoder = TableEncoder(properties!.asCodingTableKeys(), on: coreStatement)
            for var object in objects {
                try encoder.bind(object, isReplace: isReplace)
                try coreStatement.step()
                if !isReplace && object.isAutoIncrement {
                    object.lastInsertedRowID = coreStatement.lastInsertedRowID
                }
                try coreStatement.reset()
            }
        }
        return objects.count == 1 ? try doInsertObject() : try core.run(embeddedTransaction: doInsertObject )
    }
}

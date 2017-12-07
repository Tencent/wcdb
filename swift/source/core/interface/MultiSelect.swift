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

public class MultiSelect: SelectBase {
    private let keys: [CodingTableKeyBase]
    
    init(with core: Core, on propertyConvertibleList: [PropertyConvertible], tables: [String], isDistinct: Bool = false) throws {
        guard propertyConvertibleList.count > 0 else {
            throw Error.reportInterface(tag: core.tag, path: core.path, operation: .Select, code: .Misuse, message: "Selecting nothing from \(tables) is invalid")
        }
        guard tables.count > 0 else {
            throw Error.reportInterface(tag: core.tag, path: core.path, operation: .Select, code: .Misuse, message: "Empty table")
        }
        self.keys = propertyConvertibleList.map({ (propertyConvertible) -> CodingTableKeyBase in
            let codingTableKey = propertyConvertible.codingTableKey 
            guard codingTableKey.rootType is TableDecodableBase.Type else {
                Error.abort("")
            }
            return codingTableKey
        })
        
        super.init(with: core)
        statement.select(distinct: isDistinct, propertyConvertibleList).from(tables)
    }    
    
    private typealias Generator = () throws -> TableDecodableBase
    private lazy var generators: [String:Generator] = {
        var mappedKeys: [String:(type: TableDecodableBase.Type, keys: [CodingTableKeyBase])] = [:]
        let coreStatement = try! lazyCoreStatement()
        for (index, key) in keys.enumerated() {
            let tableName = coreStatement.columnTableName(atIndex: index)
            var keys: (type: TableDecodableBase.Type, keys: [CodingTableKeyBase])! = mappedKeys[tableName]
            if keys == nil {
                keys = (key.rootType as! TableDecodableBase.Type, [key])
            }
            mappedKeys[tableName] = keys
        }
        var generators: [String:Generator] = [:]
        for (tableName, mappedKey) in mappedKeys {
            let decoder = TableDecoder(mappedKey.keys, on: coreStatement)
            let generator = { () throws -> TableDecodableBase in 
                return try mappedKey.type.init(from: decoder)
            }
            generators[tableName] = generator
        }
        return generators
    }()
    
    private func extractMultiObject() throws -> [String:TableDecodableBase] {
        var multiObject: [String:TableDecodableBase] = [:]
        for (tableName, generator) in generators {
            multiObject[tableName] = try generator()
        }
        return multiObject
    }

    public func nextMultiObject() throws -> [String:TableDecodableBase]? {
        guard try next() else {
            return nil
        }
        return try extractMultiObject()
    }    
        
    public func allMultiObjects() throws -> [[String:TableDecodableBase]] {
        var multiObjects: [[String:TableDecodableBase]] = []
        while try next() {
            multiObjects.append(try extractMultiObject())
        }
        return multiObjects
    }
}

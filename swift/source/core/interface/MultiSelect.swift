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

public final class MultiSelect: SelectBase {
    private let keys: [CodingTableKeyBase]

    init(with core: Core,
         on propertyConvertibleList: [PropertyConvertible],
         tables: [String],
         isDistinct: Bool = false) throws {
        guard propertyConvertibleList.count > 0 else {
            throw Error.reportInterface(tag: core.tag,
                                        path: core.path,
                                        operation: .select,
                                        code: .misuse,
                                        message: "Selecting nothing from \(tables) is invalid")
        }
        guard tables.count > 0 else {
            throw Error.reportInterface(tag: core.tag,
                                        path: core.path,
                                        operation: .select,
                                        code: .misuse,
                                        message: "Empty table")
        }
        self.keys = propertyConvertibleList.map({ (propertyConvertible) -> CodingTableKeyBase in
            let codingTableKey = propertyConvertible.codingTableKey
            Error.assert(codingTableKey.rootType is TableDecodableBase.Type, message: "")
            return codingTableKey
        })

        super.init(with: core)
        statement.select(distinct: isDistinct, propertyConvertibleList).from(tables)
    }

    private typealias Generator = () throws -> TableDecodableBase

    struct TypedIndexedKeys {
        let type: TableDecodableBase.Type
        var indexedKeys: [String: Int]
        init(_ type: TableDecodableBase.Type, key: String, index: Int) {
            self.type = type
            self.indexedKeys = [key: index]
        }
    }
    private lazy var generators: [String: Generator] = {
        var mappedKeys: [String: TypedIndexedKeys] = [:]
        let coreStatement = try? lazyCoreStatement()
        Error.assert(coreStatement != nil, message: "")
        for (index, key) in keys.enumerated() {
            let tableName = coreStatement!.columnTableName(atIndex: index)
            var typedIndexedKeys: TypedIndexedKeys! = mappedKeys[tableName]
            if typedIndexedKeys == nil {
                let tableDecodableType = key.rootType as? TableDecodableBase.Type
                Error.assert(tableDecodableType != nil, message: "")
                typedIndexedKeys = TypedIndexedKeys(tableDecodableType!, key: key.stringValue, index: index)
            } else {
                typedIndexedKeys.indexedKeys[key.stringValue] = index
            }
            mappedKeys[tableName] = typedIndexedKeys
        }
        var generators: [String: Generator] = [:]
        for (tableName, typedIndexedKey) in mappedKeys {
            let decoder = TableDecoder(typedIndexedKey.indexedKeys, on: coreStatement!)
            let type = typedIndexedKey.type
            let generator = { () throws -> TableDecodableBase in
                return try type.init(from: decoder)
            }
            generators[tableName] = generator
        }
        return generators
    }()

    private func extractMultiObject() throws -> [String: TableDecodableBase] {
        var multiObject: [String: TableDecodableBase] = [:]
        for (tableName, generator) in generators {
            multiObject[tableName] = try generator()
        }
        return multiObject
    }

    public func nextMultiObject() throws -> [String: TableDecodableBase]? {
        guard try next() else {
            return nil
        }
        return try extractMultiObject()
    }

    public func allMultiObjects() throws -> [[String: TableDecodableBase]] {
        var multiObjects: [[String: TableDecodableBase]] = []
        while try next() {
            multiObjects.append(try extractMultiObject())
        }
        return multiObjects
    }
}

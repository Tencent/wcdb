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

/// Chain call for multi-selecting
public final class MultiSelect: Selectable {
    private let properties: [PropertyConvertible]
    private lazy var keys: [CodingTableKeyBase] = {
        self.properties.asCodingTableKeys()
    }()

    init(with handle: Handle,
         on propertyConvertibleList: [PropertyConvertible],
         tables: [String],
         isDistinct: Bool = false) {
        properties = propertyConvertibleList
        let statement = StatementSelect().select(distinct: isDistinct, propertyConvertibleList).from(tables)
        super.init(with: handle, statement: statement)
    }

    private typealias Generator = () throws -> TableDecodableBase

    struct TypedIndexedKeys {
        let type: TableDecodableBase.Type
        var indexedKeys: TableDecoder.HashedKey
        init(_ type: TableDecodableBase.Type, key: String, index: Int) {
            self.type = type
            self.indexedKeys = [key.hashValue: index]
        }
    }
    private lazy var generators: [String: Generator] = {
        var mappedKeys: [String: TypedIndexedKeys] = [:]
        if !handle.isPrepared {
            do {
                try handle.prepare(statement)
            } catch let error {
                assert(false,
                       "It should not be failed. If you think it's a bug, please report an issue to us.")
            }
        }
        for (index, key) in keys.enumerated() {
            let tableName = handle.columnTableName(atIndex: index)
            var typedIndexedKeys: TypedIndexedKeys! = mappedKeys[tableName]
            if typedIndexedKeys == nil {
                let tableDecodableType = key.rootType as? TableDecodableBase.Type
                assert(tableDecodableType != nil,
                       "\(key.rootType) must conform to TableDecodable protocol.")
                typedIndexedKeys = TypedIndexedKeys(tableDecodableType!, key: key.stringValue, index: index)
            } else {
                typedIndexedKeys.indexedKeys[key.stringValue.hashValue] = index
            }
            mappedKeys[tableName] = typedIndexedKeys
        }
        var generators: [String: Generator] = [:]
        for (tableName, typedIndexedKey) in mappedKeys {
            let decoder = TableDecoder(typedIndexedKey.indexedKeys, on: handle)
            let type = typedIndexedKey.type
            let generator = { () throws -> TableDecodableBase in
                return try type.init(from: decoder)
            }
            generators[tableName] = generator
        }
        return generators
    }()

    private func extractMultiObject() throws -> [String: Any] {
        var multiObject: [String: Any] = [:]
#if WCDB_SWIFT_BRIDGE_OBJC
        if properties[0].isSwiftProperty() {
            for (tableName, generator) in generators {
                multiObject[tableName] = try generator()
            }
        } else {
            try lazyPrepareStatement()
            multiObject = WCTAPIBridge.extractMultiObject(onResultColumns: properties.asWCTBridgeProperties(), from: handle.getRawStatement())!
        }
#else
        assert(properties[0].isSwiftProperty())
        for (tableName, generator) in generators {
            multiObject[tableName] = try generator()
        }
#endif
        return multiObject
    }

    /// Get next selected object. You can do an iteration using it.
    ///
    ///     while let multiObject = try self.nextMultiObject() {
    ///         let object1 = multiObject[tableName1]
    ///         let object2 = multiObject[tableName2]
    ///         //...
    ///     }
    ///
    /// - Returns: Mapping from table name to object. Nil means the end of iteration.
    /// - Throws: `Error`
    public func nextMultiObject() throws -> [String: Any]? {
        try lazyPrepareStatement()
        guard try next() else {
            return nil
        }
        return try extractMultiObject()
    }

    /// Get all selected objects.
    ///
    /// - Returns: Array contained mapping from table name to object.
    /// - Throws: `Error`
    public func allMultiObjects() throws -> [[String: Any]] {
        var multiObjects: [[String: Any]] = []
        try lazyPrepareStatement()
        while try next() {
            multiObjects.append(try extractMultiObject())
        }
        return multiObjects
    }
}

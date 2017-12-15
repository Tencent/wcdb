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

final class TableEncoder: Encoder {
    private final class KeyedEncodingTableContainer<CodingTableKeyType: CodingKey>: KeyedEncodingContainerProtocol {
        typealias Key = CodingTableKeyType

        let codingPath: [CodingKey] = []

        let coreStatement: CoreStatement
        let indexedCodingTableKeys: [String: Int]
        let primaryKey: CodingTableKeyBase?

        init(with indexedCodingTableKeys: [String: Int],
             on coreStatement: CoreStatement,
             and primaryKey: CodingTableKeyBase?,
             keyType: CodingTableKeyType.Type) {
            self.indexedCodingTableKeys = indexedCodingTableKeys
            self.coreStatement = coreStatement
            self.primaryKey = primaryKey
        }

        func bindingIndex(by codingTableKey: Key) -> Int? {
            return indexedCodingTableKeys[codingTableKey.stringValue]
        }

        func superEncoder() -> Swift.Encoder {
            Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
        }

        func superEncoder(forKey key: Key) -> Swift.Encoder {
            Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
        }

        func encodePrimaryKeyIfPresent(forKey key: Key, atIndex index: Int) -> Bool {
            guard key.stringValue == primaryKey?.stringValue else {
                return false
            }
            coreStatement.bind(nil, toIndex: index)
            return true
        }

        public func generalEncode(_ value: ColumnEncodableBase?, forKey key: Key) {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }

        public func generalEncode(_ value: ColumnEncodableBase, forKey key: Key) {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }

        func encodeNil(forKey key: Key) throws {
            generalEncode(nil, forKey: key)
        }

        func encode(_ value: Int, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: Bool, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: Float, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: Double, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: String, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode<Object>(_ value: Object, forKey key: Key) throws where Object: Encodable {
            let encodableColumnValue = value as? ColumnEncodableBase
            Error.assert(encodableColumnValue != nil, message: "[\(key)] must conform to ColumnEncodable protocol.")
            generalEncode(encodableColumnValue!, forKey: key)
        }

        func encode(_ value: Int8, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: Int16, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: Int32, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: Int64, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: UInt, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: UInt8, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: UInt16, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: UInt32, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encode(_ value: UInt64, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: Bool?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: Int?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: Int8?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: Int16?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: Int32?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: Int64?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: UInt?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: UInt8?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: UInt16?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: UInt32?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: UInt64?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: Float?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: Double?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent(_ value: String?, forKey key: Key) throws {
            generalEncode(value, forKey: key)
        }

        func encodeIfPresent<Object>(_ value: Object?,
                                     forKey key: Key) throws
            where Object: Encodable {
            Error.assert(Object.self is ColumnEncodableBase.Type,
                         message: "[\(key)] must conform to ColumnEncodable protocol.")
            generalEncode(value as? ColumnEncodableBase, forKey: key)
        }

        func nestedContainer<NestedKey>(keyedBy keyType: NestedKey.Type,
                                        forKey key: Key) -> KeyedEncodingContainer<NestedKey>
            where NestedKey: CodingKey {
            Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
        }

        func nestedUnkeyedContainer(forKey key: Key) -> UnkeyedEncodingContainer {
            Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
        }
    }

    let codingPath: [CodingKey] = []
    let userInfo: [CodingUserInfoKey: Any] = [:]

    private let coreStatement: CoreStatement
    private let indexedCodingTableKeys: [String: Int]
    private var primaryKey: CodingTableKeyBase?

    func singleValueContainer() -> SingleValueEncodingContainer {
        Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
    }

    func unkeyedContainer() -> UnkeyedEncodingContainer {
        Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
    }

    func container<Key>(keyedBy type: Key.Type) -> KeyedEncodingContainer<Key> where Key: CodingKey {
        Error.assert(Key.self is CodingTableKeyBase.Type,
                     message: "[\(Key.self)] must conform to CodingTableKey protocol.")
        let container = KeyedEncodingTableContainer(with: indexedCodingTableKeys,
                                                    on: coreStatement,
                                                    and: primaryKey,
                                                    keyType: Key.self)
        return KeyedEncodingContainer(container)
    }

    convenience init(_ codingTableKeys: [CodingTableKeyBase], on coreStatement: CoreStatement) {
        var indexedCodingTableKeys: [String: Int] = [:]
        for (index, codingTableKey) in codingTableKeys.enumerated() {
            indexedCodingTableKeys[codingTableKey.stringValue] = index + 1
        }
        self.init(indexedCodingTableKeys, on: coreStatement)
    }

    init(_ indexedCodingTableKeys: [String: Int], on coreStatement: CoreStatement) {
        self.indexedCodingTableKeys = indexedCodingTableKeys
        self.coreStatement = coreStatement
    }

    func bind<TableEncodableType: TableEncodable>(_ object: TableEncodableType, isReplace: Bool = false) throws {
        if !isReplace && object.isAutoIncrement {
            primaryKey = TableEncodableType.CodingKeys.objectRelationalMapping.getPrimaryKey()
        }
        try object.encode(to: self)
        primaryKey = nil
    }
}

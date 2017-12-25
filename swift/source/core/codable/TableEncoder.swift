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

final class TableEncoder<TableEncodableType: TableEncodable>: Encoder {
    private final class KeyedEncodingTableContainer: KeyedEncodingContainerProtocol {
        typealias Key = TableEncodableType.CodingKeys

        let codingPath: [CodingKey] = []

        let recyclableHandleStatement: RecyclableHandleStatement
        let indexedCodingTableKeys: [String: Int]
        let handleStatement: HandleStatement
        var primaryKey: CodingTableKeyBase?

        init(with indexedCodingTableKeys: [String: Int],
             on recyclableHandleStatement: RecyclableHandleStatement) {
            self.indexedCodingTableKeys = indexedCodingTableKeys
            self.recyclableHandleStatement = recyclableHandleStatement
            self.handleStatement = recyclableHandleStatement.raw
        }

        func bindingIndex(by codingTableKey: Key) -> Int? {
            return indexedCodingTableKeys[codingTableKey.stringValue]
        }

        func superEncoder() -> Swift.Encoder {
            Error.abort("It should not be called. If you think it's a bug, please report an issue to us.")
        }

        func superEncoder(forKey key: Key) -> Swift.Encoder {
            Error.abort("It should not be called. If you think it's a bug, please report an issue to us.")
        }

        func encodePrimaryKeyIfPresent(forKey key: Key, atIndex index: Int) -> Bool {
            guard key.stringValue == primaryKey?.stringValue else {
                return false
            }
            handleStatement.bind(nil, toIndex: index)
            return true
        }

        public func generalEncode(_ value: ColumnEncodableBase?, forKey key: Key) {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                handleStatement.bind(value?.archivedFundamentalValue(), toIndex: bindingIndex)
            }
        }

        public func generalEncode(_ value: ColumnEncodableBase, forKey key: Key) {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                handleStatement.bind(value.archivedFundamentalValue(), toIndex: bindingIndex)
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
            Error.abort("It should not be called. If you think it's a bug, please report an issue to us.")
        }

        func nestedUnkeyedContainer(forKey key: Key) -> UnkeyedEncodingContainer {
            Error.abort("It should not be called. If you think it's a bug, please report an issue to us.")
        }
    }

    let codingPath: [CodingKey] = []
    let userInfo: [CodingUserInfoKey: Any] = [:]

    func singleValueContainer() -> SingleValueEncodingContainer {
        Error.abort("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    func unkeyedContainer() -> UnkeyedEncodingContainer {
        Error.abort("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    func container<Key>(keyedBy type: Key.Type) -> KeyedEncodingContainer<Key> where Key: CodingKey {
        return KeyedEncodingContainer(container) as! KeyedEncodingContainer<Key>
    }

    private let container: KeyedEncodingTableContainer

    convenience init(_ codingTableKeys: [CodingTableKeyBase],
                     on recyclableHandleStatement: RecyclableHandleStatement) {
        var indexedCodingTableKeys: [String: Int] = [:]
        for (index, codingTableKey) in codingTableKeys.enumerated() {
            indexedCodingTableKeys[codingTableKey.stringValue] = index + 1
        }
        self.init(indexedCodingTableKeys, on: recyclableHandleStatement)
    }

    init(_ indexedCodingTableKeys: [String: Int], on recyclableHandleStatement: RecyclableHandleStatement) {
        container = KeyedEncodingTableContainer(with: indexedCodingTableKeys, on: recyclableHandleStatement)
    }

    func bind(_ object: TableEncodableType, isReplace: Bool = false) throws {
        if !isReplace && object.isAutoIncrement {
            container.primaryKey = TableEncodableType.CodingKeys.objectRelationalMapping.getPrimaryKey()
        } else {
            container.primaryKey = nil
        }
        try object.encode(to: self)
    }
}

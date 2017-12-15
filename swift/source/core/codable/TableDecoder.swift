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

final class TableDecoder: Decoder {
    private final class KeyedDecodingTableContainer<CodingTableKeyType: CodingKey> : KeyedDecodingContainerProtocol {
        typealias Key = CodingTableKeyType

        let codingPath: [CodingKey] = []

        let coreStatement: CoreStatement
        let indexedCodingTableKeys: [String: Int]

        init(with indexedCodingTableKeys: [String: Int], on coreStatement: CoreStatement, and keyType: Key.Type) {
            self.indexedCodingTableKeys = indexedCodingTableKeys
            self.coreStatement = coreStatement
        }

        func columnIndex(by key: Key) -> Int? {
            return indexedCodingTableKeys[key.stringValue]
        }

        var allKeys: [Key] {
            return []
        }

        func contains(_ key: Key) -> Bool {
            return columnIndex(by: key) != nil
        }

        //Decode
        func decodeNil(forKey key: Key) throws -> Bool {
            guard let index = columnIndex(by: key) else {
                return false
            }
            return coreStatement.value(atIndex: index) ?? false
        }

        func decode(_ type: Bool.Type, forKey key: Key) throws -> Bool {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? false
        }

        func decode(_ type: Int.Type, forKey key: Key) throws -> Int {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: Int8.Type, forKey key: Key) throws -> Int8 {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: Int16.Type, forKey key: Key) throws -> Int16 {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: Int32.Type, forKey key: Key) throws -> Int32 {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: Int64.Type, forKey key: Key) throws -> Int64 {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: UInt.Type, forKey key: Key) throws -> UInt {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: UInt8.Type, forKey key: Key) throws -> UInt8 {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: UInt16.Type, forKey key: Key) throws -> UInt16 {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: UInt32.Type, forKey key: Key) throws -> UInt32 {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: UInt64.Type, forKey key: Key) throws -> UInt64 {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: Float.Type, forKey key: Key) throws -> Float {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: Double.Type, forKey key: Key) throws -> Double {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? 0
        }

        func decode(_ type: String.Type, forKey key: Key) throws -> String {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            return coreStatement.value(atIndex: index!) ?? ""
        }

        func decode<Object>(_ type: Object.Type, forKey key: Key) throws -> Object where Object: Decodable {
            let index = columnIndex(by: key)
            Error.assert(index != nil, message: "If [\(key)] would not be decoded, please make it optional.")
            let decodableType = Object.self as? ColumnDecodableBase.Type
            Error.assert(decodableType != nil, message: "[\(key)] must conform to ColumnDecodable protocol.")
            guard let object = coreStatement.value(atIndex: index!, of: decodableType!) as? Object else {
                throw Error.reportCore(tag: coreStatement.tag,
                                       path: coreStatement.path,
                                       operation: .encode,
                                       code: .misuse,
                                       message: "If [\(key)] would be decoded as nil, please make it optional.")
            }
            return object
        }

        //Decode if present
        func decodeIfPresent(_ type: Bool.Type, forKey key: Key) throws -> Bool? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: Int.Type, forKey key: Key) throws -> Int? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: Int8.Type, forKey key: Key) throws -> Int8? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: Int16.Type, forKey key: Key) throws -> Int16? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: Int32.Type, forKey key: Key) throws -> Int32? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: Int64.Type, forKey key: Key) throws -> Int64? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: UInt.Type, forKey key: Key) throws -> UInt? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: UInt8.Type, forKey key: Key) throws -> UInt8? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: UInt16.Type, forKey key: Key) throws -> UInt16? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: UInt32.Type, forKey key: Key) throws -> UInt32? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: UInt64.Type, forKey key: Key) throws -> UInt64? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: Float.Type, forKey key: Key) throws -> Float? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: Double.Type, forKey key: Key) throws -> Double? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent(_ type: String.Type, forKey key: Key) throws -> String? {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            return coreStatement.value(atIndex: index)
        }

        func decodeIfPresent<Object>(_ type: Object.Type, forKey key: Key) throws -> Object? where Object: Decodable {
            guard let index = columnIndex(by: key) else {
                return nil
            }
            let decodableType = Object.self as? ColumnDecodableBase.Type
            Error.assert(decodableType != nil, message: "[\(key)] must conform to ColumnDecodable protocol.")
            return coreStatement.value(atIndex: index, of: decodableType!) as? Object
        }

        func nestedContainer<NestedKey>(keyedBy type: NestedKey.Type,
                                        forKey key: Key) throws -> KeyedDecodingContainer<NestedKey>
            where NestedKey: CodingKey {
            Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
        }

        func nestedUnkeyedContainer(forKey key: Key) throws -> UnkeyedDecodingContainer {
            Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
        }

        func superDecoder() throws -> Swift.Decoder {
            Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
        }

        func superDecoder(forKey key: Key) throws -> Swift.Decoder {
            Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
        }
    }

    let codingPath: [CodingKey] = []
    let userInfo: [CodingUserInfoKey: Any] = [:]

    private let coreStatement: CoreStatement
    private let indexedCodingTableKeys: [String: Int]

    convenience init(_ codingTableKeys: [CodingTableKeyBase], on coreStatement: CoreStatement) {
        var indexedCodingTableKeys: [String: Int] = [:]
        for (index, codingTableKey) in codingTableKeys.enumerated() {
            indexedCodingTableKeys[codingTableKey.stringValue] = index
        }
        self.init(indexedCodingTableKeys, on: coreStatement)
    }

    init(_ indexedCodingTableKeys: [String: Int], on coreStatement: CoreStatement) {
        self.indexedCodingTableKeys = indexedCodingTableKeys
        self.coreStatement = coreStatement
    }

    func container<Key>(keyedBy type: Key.Type) throws -> KeyedDecodingContainer<Key> where Key: CodingKey {
        Error.assert(Key.self is CodingTableKeyBase.Type,
                     message: "[\(Key.self)] must conform to CodingTableKey protocol.")
        let container = KeyedDecodingTableContainer(with: indexedCodingTableKeys,
                                                    on: coreStatement,
                                                    and: Key.self)
        return KeyedDecodingContainer(container)
    }

    func unkeyedContainer() throws -> UnkeyedDecodingContainer {
        Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
    }

    func singleValueContainer() throws -> SingleValueDecodingContainer {
        Error.abort("It should be called. If you think it's a bug, please report an issue to us.")
    }
}

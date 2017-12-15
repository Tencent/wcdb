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

final class ColumnTypeEncoder: Encoder {
    private final class KeyedEncodingTypedContainer<CodingsKey: CodingKey>: KeyedEncodingContainerProtocol {
        typealias Key = CodingsKey

        let encoder: ColumnTypeEncoder
        let codingPath: [CodingKey] = []

        init(encoder: ColumnTypeEncoder) {
            self.encoder = encoder
        }

        func superEncoder() -> Swift.Encoder {
            Error.abort("")
        }

        func superEncoder(forKey key: Key) -> Swift.Encoder {
            Error.abort("")
        }

        func encodeNil(forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encode(_ value: Int, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer64
        }

        func encode(_ value: Bool, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encode(_ value: Float, forKey key: Key) throws {
            encoder.results[key.stringValue] = .float
        }

        func encode(_ value: Double, forKey key: Key) throws {
            encoder.results[key.stringValue] = .float
        }

        func encode(_ value: String, forKey key: Key) throws {
            encoder.results[key.stringValue] = .text
        }

        func encode<Object>(_ value: Object, forKey key: Key) throws where Object: Swift.Encodable {
            if let columnEncodableObject = Object.self as? ColumnCodableBase.Type {
                encoder.results[key.stringValue] = columnEncodableObject.columnType
            } else {
                encoder.results[key.stringValue] = .BLOB
            }
        }

        func encode(_ value: Int8, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encode(_ value: Int16, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encode(_ value: Int32, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encode(_ value: Int64, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer64
        }

        func encode(_ value: UInt, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer64
        }

        func encode(_ value: UInt8, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encode(_ value: UInt16, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encode(_ value: UInt32, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encode(_ value: UInt64, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer64
        }

        func nestedContainer<NestedKey>(keyedBy keyType: NestedKey.Type,
                                        forKey key: Key) -> KeyedEncodingContainer<NestedKey>
            where NestedKey: CodingKey {
            Error.abort("")
        }

        func nestedUnkeyedContainer(forKey key: Key) -> UnkeyedEncodingContainer {
            Error.abort("")
        }

        func encodeIfPresent(_ value: Bool?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encodeIfPresent(_ value: Int?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer64
        }

        func encodeIfPresent(_ value: Int8?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encodeIfPresent(_ value: Int16?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encodeIfPresent(_ value: Int32?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encodeIfPresent(_ value: Int64?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer64
        }

        func encodeIfPresent(_ value: UInt?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer64
        }

        func encodeIfPresent(_ value: UInt8?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encodeIfPresent(_ value: UInt16?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encodeIfPresent(_ value: UInt32?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer32
        }

        func encodeIfPresent(_ value: UInt64?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .integer64
        }

        func encodeIfPresent(_ value: Float?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .float
        }

        func encodeIfPresent(_ value: Double?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .float
        }

        func encodeIfPresent(_ value: String?, forKey key: Key) throws {
            encoder.results[key.stringValue] = .text
        }

        func encodeIfPresent<Object>(_ value: Object?, forKey key: Key) throws where Object: Encodable {
            if let columnEncodableObject = Object.self as? ColumnCodableBase.Type {
                encoder.results[key.stringValue] = columnEncodableObject.columnType
            } else {
                encoder.results[key.stringValue] = .BLOB
            }
        }
    }

    let codingPath: [CodingKey] = []
    let userInfo: [CodingUserInfoKey: Any] = [:]

    private var results: [String: ColumnType] = [:]

    func singleValueContainer() -> SingleValueEncodingContainer {
        Error.abort("")
    }

    func unkeyedContainer() -> UnkeyedEncodingContainer {
        Error.abort("")
    }

    func container<Key>(keyedBy type: Key.Type) -> KeyedEncodingContainer<Key> where Key: CodingKey {
        return KeyedEncodingContainer(KeyedEncodingTypedContainer(encoder: self))
    }

    static func getColumnTypes(of type: TableEncodableBase.Type) -> [String: ColumnType] {
        let encoder = ColumnTypeEncoder()
        let dummy = type.init()
        Error.assert(((try? dummy.encode(to: encoder)) != nil), message: "")
        return encoder.results
    }
}

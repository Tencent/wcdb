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

final class ColumnTypeDecoder: Decoder {

    private var results: [String: ColumnType] = [:]

    static func types(of type: TableDecodableBase.Type) -> [String: ColumnType] {
        let decoder = ColumnTypeDecoder()
        _ = try? type.init(from: decoder)
        return decoder.results
    }

    func container<Key>(keyedBy type: Key.Type) throws -> KeyedDecodingContainer<Key> where Key: CodingKey {
        return KeyedDecodingContainer(ColumnTypeDecodingContainer<Key>(with: self))
    }

    private final class ColumnTypeDecodingContainer<CodingKeys: CodingKey>: KeyedDecodingContainerProtocol {

        typealias Key = CodingKeys

        private let decoder: ColumnTypeDecoder

        private struct SizedPointer {
            private let pointer: UnsafeMutableRawPointer
            private let size: Int
            init<T>(of type: T.Type = T.self) {
                size = MemoryLayout<T>.size
                pointer = UnsafeMutableRawPointer.allocate(bytes: size, alignedTo: size)
                memset(pointer, 0, size)
            }
            func deallocate() {
                pointer.deallocate(bytes: size, alignedTo: size)
            }
            func getPointee<T>(of type: T.Type = T.self) -> T {
                return pointer.assumingMemoryBound(to: type).pointee
            }
        }

        private var sizedPointers: ContiguousArray<SizedPointer>

        init(with decoder: ColumnTypeDecoder) {
            self.decoder = decoder
            self.sizedPointers = ContiguousArray<SizedPointer>()
        }

        deinit {
            for sizedPointer in sizedPointers {
                sizedPointer.deallocate()
            }
        }

        func contains(_ key: Key) -> Bool {
            return true
        }

        func decodeNil(forKey key: Key) throws -> Bool {
            decoder.results[key.stringValue] = Bool.columnType
            return false
        }

        func decode(_ type: Bool.Type, forKey key: Key) throws -> Bool {
            decoder.results[key.stringValue] = type.columnType
            return false
        }

        func decode(_ type: Int.Type, forKey key: Key) throws -> Int {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: Int8.Type, forKey key: Key) throws -> Int8 {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: Int16.Type, forKey key: Key) throws -> Int16 {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: Int32.Type, forKey key: Key) throws -> Int32 {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: Int64.Type, forKey key: Key) throws -> Int64 {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: UInt.Type, forKey key: Key) throws -> UInt {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: UInt8.Type, forKey key: Key) throws -> UInt8 {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: UInt16.Type, forKey key: Key) throws -> UInt16 {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: UInt32.Type, forKey key: Key) throws -> UInt32 {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: UInt64.Type, forKey key: Key) throws -> UInt64 {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: Float.Type, forKey key: Key) throws -> Float {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: Double.Type, forKey key: Key) throws -> Double {
            decoder.results[key.stringValue] = type.columnType
            return 0
        }

        func decode(_ type: String.Type, forKey key: Key) throws -> String {
            decoder.results[key.stringValue] = type.columnType
            return ""
        }

        func decode<T>(_ type: T.Type, forKey key: Key) throws -> T where T: Decodable {
            // `type` must conform to ColumnDecodableBase protocol
            let columnDecodableType = type as! ColumnDecodableBase.Type
            decoder.results[key.stringValue] = columnDecodableType.columnType

            let sizedPointer = SizedPointer(of: T.self)
            sizedPointers.append(sizedPointer)
            return sizedPointer.getPointee()
        }

        var codingPath: [CodingKey] {
            fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
        }

        var allKeys: [Key] {
            fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
        }

        func nestedContainer<NestedKey>(keyedBy type: NestedKey.Type,
                                        forKey key: Key)
            throws -> KeyedDecodingContainer<NestedKey> where NestedKey: CodingKey {
            fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
        }

        func nestedUnkeyedContainer(forKey key: Key) throws -> UnkeyedDecodingContainer {
            fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
        }

        func superDecoder() throws -> Decoder {
            fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
        }

        func superDecoder(forKey key: Key) throws -> Decoder {
            fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
        }
    }

    var codingPath: [CodingKey] {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    var userInfo: [CodingUserInfoKey: Any] {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    func unkeyedContainer() throws -> UnkeyedDecodingContainer {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    func singleValueContainer() throws -> SingleValueDecodingContainer {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }
}

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

/*
 * Builtin codable implementation
 *
 * .integer32:
 *      Bool, Int, Int8, Int16, Int32, UInt, UInt8, UInt16, UInt32  
 * .integer64:
 *      Int64, UInt64, Date
 * .float:
 *      Float, Double
 * .text:
 *      String, URL
 * .BLOB:
 *      Data, Array, Dictionary, Set
 *
 */

//Bultin Column Codable
extension Int8: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = value.toInt8()
    }
    public func archivedValue() -> Int32? {
        return self.toInt32()
    }
}

extension Int16: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = value.toInt16()
    }
    public func archivedValue() -> Int32? {
        return self.toInt32()
    }
}

extension Int32: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = value
    }
    public func archivedValue() -> Int32? {
        return self
    }
}

extension Int64: ColumnCodable {
    public typealias FundamentalType = Int64
    public init?(with value: Int64) {
        self = value
    }
    public func archivedValue() -> Int64? {
        return self
    }
}

extension Int: ColumnCodable {
    public typealias FundamentalType = Int64
    public init?(with value: Int64) {
        self = value.toInt()
    }
    public func archivedValue() -> Int64? {
        return self.toInt64()
    }
}

extension UInt8: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = value.toUInt8()
    }
    public func archivedValue() -> Int32? {
        return self.toInt32()
    }
}

extension UInt16: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = value.toUInt16()
    }
    public func archivedValue() -> Int32? {
        return self.toInt32()
    }
}

extension UInt32: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = value.toUInt32()
    }
    public func archivedValue() -> Int32? {
        return self.toInt32()
    }
}

extension UInt64: ColumnCodable {
    public typealias FundamentalType = Int64
    public init?(with value: Int64) {
        self = value.toUInt64()
    }
    public func archivedValue() -> Int64? {
        return self.toInt64()
    }
}

extension UInt: ColumnCodable {
    public typealias FundamentalType = Int64
    public init?(with value: Int64) {
        self = value.toUInt()
    }
    public func archivedValue() -> Int64? {
        return self.toInt64()
    }
}

extension Bool: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = value.toBool()
    }
    public func archivedValue() -> Int32? {
        return self.toInt32()
    }
}

extension Float: ColumnCodable {
    public typealias FundamentalType = Double
    public init?(with value: Double) {
        self = value.toFloat()
    }
    public func archivedValue() -> Double? {
        return self.toDouble()
    }
}

extension Double: ColumnCodable {
    public typealias FundamentalType = Double
    public init?(with value: Double) {
        self = value
    }
    public func archivedValue() -> Double? {
        return self
    }
}

extension String: ColumnCodable {
    public typealias FundamentalType = String
    public init?(with value: String) {
        self = value
    }
    public func archivedValue() -> String? {
        return self
    }
}

extension Data: ColumnCodable {
    public typealias FundamentalType = Data
    public init?(with value: Data) {
        self = value
    }
    public func archivedValue() -> Data? {
        return self
    }
}

extension Date: ColumnCodable {
    public typealias FundamentalType = Int64
    public init?(with value: Int64) {
        self.init(timeIntervalSince1970: TimeInterval(value))
    }
    public func archivedValue() -> Int64? {
        return Int64(timeIntervalSince1970)
    }
}

extension URL: ColumnCodable {
    public typealias FundamentalType = String
    public init?(with value: String) {
        self.init(string: value)
    }
    public func archivedValue() -> String? {
        return self.absoluteString
    }
}

extension Array: CollectionColumnCodable {
    public init?(with value: Data) {
        guard let decodable = try? JSONDecoder().decode(Array.self, from: value) else {
            return nil
        }
        self = decodable
    }
    public func archivedValue() -> Data? {
        return try? JSONEncoder().encode(self)
    }
}

extension Dictionary: CollectionColumnCodable {
    public init?(with value: Data) {
        guard let decodable = try? JSONDecoder().decode(Dictionary.self, from: value) else {
            return nil
        }
        self = decodable
    }
    public func archivedValue() -> Data? {
        return try? JSONEncoder().encode(self)
    }
}

extension Set: CollectionColumnCodable {
    public init?(with value: Data) {
        guard let decodable = try? JSONDecoder().decode(Set.self, from: value) else {
            return nil
        }
        self = decodable
    }
    public func archivedValue() -> Data? {
        return try? JSONEncoder().encode(self)
    }
}

//JSONCodable
public protocol ColumnJSONEncodable: ColumnEncodable where FundamentalType == Data {}
extension ColumnJSONEncodable {
    public func archivedValue() -> FundamentalType? {
        return try? JSONEncoder().encode(self)
    }
}

public protocol ColumnJSONDecodable: ColumnDecodable where FundamentalType == Data {}
extension ColumnJSONDecodable {
    public init?(with value: FundamentalType) {
        guard value.count > 0 else {
            return nil
        }
        guard let object = try? JSONDecoder().decode(Self.self, from: value) else {
            return nil
        }
        self = object
    }
}

public protocol ColumnJSONCodable: ColumnJSONEncodable, ColumnJSONDecodable {}
extension ColumnJSONCodable {
    public typealias FundamentalType = Data
    public static var columnType: ColumnType {
        return .BLOB
    }
}

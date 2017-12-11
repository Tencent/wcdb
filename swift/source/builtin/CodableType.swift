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
 * Bool, Int, Int8, Int16, Int32, Int64, UInt, UInt8, UInt16, UInt32, UInt64, Float, Double, NSNumber 
 * String
 * Data
 * Date
 * 
 * TODO: Array, Dictionary, Set, URL 
 *
 */

//Bultin Column Codable
extension Int8: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = Int8(truncatingIfNeeded: value)
    }
    public func archivedValue() -> Int32? {
        return Int32(self)
    }
}

extension Int16: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = Int16(truncatingIfNeeded: value)
    }
    public func archivedValue() -> Int32? {
        return Int32(self)
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
        self = Int(truncatingIfNeeded: value)
    }
    public func archivedValue() -> Int64? {
        return Int64(self)
    }
}

extension UInt8: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = UInt8(truncatingIfNeeded: UInt32(bitPattern: value))
    }
    public func archivedValue() -> Int32? {
        return Int32(bitPattern: UInt32(self))
    }
}

extension UInt16: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = UInt16(truncatingIfNeeded: UInt32(bitPattern: value))
    }
    public func archivedValue() -> Int32? {
        return Int32(bitPattern: UInt32(self))
    }
}

extension UInt32: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = UInt32(bitPattern: value)
    }
    public func archivedValue() -> Int32? {
        return Int32(bitPattern: self)
    }
}

extension UInt64: ColumnCodable {
    public typealias FundamentalType = Int64
    public init?(with value: Int64) {
        self = UInt64(bitPattern: value)
    }
    public func archivedValue() -> Int64? {
        return Int64(bitPattern: self)
    }
}

extension UInt: ColumnCodable {
    public typealias FundamentalType = Int64
    public init?(with value: Int64) {
        self = UInt(truncatingIfNeeded: UInt64(bitPattern: value))
    }
    public func archivedValue() -> Int64? {
        return Int64(bitPattern: UInt64(self))
    }
}

extension Bool: ColumnCodable {
    public typealias FundamentalType = Int32
    public init?(with value: Int32) {
        self = (value != 0)
    }
    public func archivedValue() -> Int32? {
        return self ? 1 : 0
    }
}

extension Float: ColumnCodable {
    public typealias FundamentalType = Double
    public init?(with value: Double) {
        self = Float(value)
    }
    public func archivedValue() -> Double? {
        return Double(self)
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
    public var columnType: ColumnType {
        return .BLOB
    }
}

//NSCodable
public protocol ColumnNSEncodable: ColumnEncodable where FundamentalType == Data {}
extension ColumnNSEncodable {
    public func archivedValue() -> FundamentalType? {
        return NSKeyedArchiver.archivedData(withRootObject: self)
    }
}

public protocol ColumnNSDecodable: ColumnDecodable where FundamentalType == Data {}
extension ColumnNSDecodable {
    public init?(with value: FundamentalType) {
        guard value.count > 0 else {
            return nil
        }
        guard let object = NSKeyedUnarchiver.unarchiveObject(with: value) else {
            return nil
        }
        guard let typedObject = object as? Self else {
            return nil
        }
        self = typedObject
    }
}

public protocol ColumnNSCodable: ColumnNSEncodable, ColumnNSDecodable {}
extension ColumnNSCodable {
    public typealias FundamentalType = Data
    public static var columnType: ColumnType {
        return .BLOB
    }
    public var columnType: ColumnType {
        return .BLOB
    }
}

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
 *      Int64, UInt64
 * .float:
 *      Float, Double, Date
 * .text:
 *      String, URL
 * .BLOB:
 *      Data, Array, Dictionary, Set
 *
 */

//Bultin Column Codable
extension Int8: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer32
    }
    public init?(with value: FundamentalValue) {
        self = value.int32Value.toInt8()
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.toInt32())
    }
}

extension Int16: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer32
    }
    public init?(with value: FundamentalValue) {
        self = value.int32Value.toInt16()
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.toInt32())
    }
}

extension Int32: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer32
    }
    public init?(with value: FundamentalValue) {
        self = value.int32Value
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self)
    }
}

extension Int64: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer64
    }
    public init?(with value: FundamentalValue) {
        self = value.int64Value
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self)
    }
}

extension Int: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer64
    }
    public init?(with value: FundamentalValue) {
        self = value.int64Value.toInt()
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.toInt64())
    }
}

extension UInt8: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer32
    }
    public init?(with value: FundamentalValue) {
        self = value.int32Value.toUInt8()
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.toInt32())
    }
}

extension UInt16: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer32
    }
    public init?(with value: FundamentalValue) {
        self = value.int32Value.toUInt16()
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.toInt32())
    }
}

extension UInt32: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer32
    }
    public init?(with value: FundamentalValue) {
        self = value.int32Value.toUInt32()
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.toInt32())
    }
}

extension UInt64: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer64
    }
    public init?(with value: FundamentalValue) {
        self = value.int64Value.toUInt64()
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.toInt64())
    }
}

extension UInt: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer64
    }
    public init?(with value: FundamentalValue) {
        self = value.int64Value.toUInt()
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.toInt64())
    }
}

extension Bool: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .integer32
    }
    public init?(with value: FundamentalValue) {
        self = value.int32Value.toBool()
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.toInt32())
    }
}

extension Float: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .float
    }
    public init?(with value: FundamentalValue) {
        self = value.doubleValue.toFloat()
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.toDouble())
    }
}

extension Double: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .float
    }
    public init?(with value: FundamentalValue) {
        self = value.doubleValue
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self)
    }
}

extension String: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .text
    }
    public init?(with value: FundamentalValue) {
        self = value.stringValue
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self)
    }
}

extension Data: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .BLOB
    }
    public init?(with value: FundamentalValue) {
        self = value.dataValue
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self)
    }
}

extension Date: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .float
    }
    public init?(with value: FundamentalValue) {
        self.init(timeIntervalSince1970: value.doubleValue)
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(timeIntervalSince1970)
    }
}

extension URL: ColumnCodable, LiteralValueConvertible, ExpressionCanBeOperated {
    public static var columnType: ColumnType {
        return .text
    }
    public init?(with value: FundamentalValue) {
        self.init(string: value.stringValue)
    }
    public func archivedValue() -> FundamentalValue {
        return FundamentalValue(self.absoluteString)
    }
}

extension Array: ColumnCodable where Element: Codable {
    public static var columnType: ColumnType {
        return .BLOB
    }
    public init?(with value: FundamentalValue) {
        guard let decodable = try? JSONDecoder().decode(Array.self, from: value.dataValue) else {
            return nil
        }
        self = decodable
    }
    public func archivedValue() -> FundamentalValue {
        guard let encoded = try? JSONEncoder().encode(self) else {
            return FundamentalValue(nil)
        }
        return FundamentalValue(encoded)
    }
}

extension Dictionary: ColumnCodable where Key: Codable, Value: Codable {
    public static var columnType: ColumnType {
        return .BLOB
    }
    public init?(with value: FundamentalValue) {
        guard let decodable = try? JSONDecoder().decode(Dictionary.self, from: value.dataValue) else {
            return nil
        }
        self = decodable
    }
    public func archivedValue() -> FundamentalValue {
        guard let encoded = try? JSONEncoder().encode(self) else {
            return FundamentalValue(nil)
        }
        return FundamentalValue(encoded)
    }
}

extension Set: ColumnCodable where Element: Codable {
    public static var columnType: ColumnType {
        return .BLOB
    }
    public init?(with value: FundamentalValue) {
        guard let decodable = try? JSONDecoder().decode(Set.self, from: value.dataValue) else {
            return nil
        }
        self = decodable
    }
    public func archivedValue() -> FundamentalValue {
        guard let encoded = try? JSONEncoder().encode(self) else {
            return FundamentalValue(nil)
        }
        return FundamentalValue(encoded)
    }
}

//JSONCodable
public protocol ColumnJSONEncodable: ColumnEncodable {}
extension ColumnJSONEncodable {
    public func archivedValue() -> FundamentalValue {
        guard let encoded = try? JSONEncoder().encode(self) else {
            return FundamentalValue(nil)
        }
        return FundamentalValue(encoded)
    }
}

public protocol ColumnJSONDecodable: ColumnDecodable {}
extension ColumnJSONDecodable {
    public init?(with value: FundamentalValue) {
        guard let decodable = try? JSONDecoder().decode(Self.self, from: value.dataValue) else {
            return nil
        }
        self = decodable
    }
}

public protocol ColumnJSONCodable: ColumnJSONEncodable, ColumnJSONDecodable {}
extension ColumnJSONCodable {
    public static var columnType: ColumnType {
        return .BLOB
    }
}

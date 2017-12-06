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

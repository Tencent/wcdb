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
 * Builtin codable implementation for
 *
 * Bool
 * Int, Int8, Int16, UInt, UInt8, UInt16, UInt32, UInt64
 * Float
 * Date
 * 
 * Note that Int32, Int64, Double, String, Data are fundamental types so they are codable too.
 * 
 */

extension Int : CodableStructColumn {
    public typealias FundamentalType = Int64
    public typealias SelfType = Int
    
    public init?(withTypedValue value: Int64?) {
        guard value != nil else {
            return nil
        }
        self = Int(value!)
    }
    
    public func archivedTypedValue() -> Int64? {
        return Int64(self)
    }    
}

extension UInt : CodableStructColumn {
    public typealias FundamentalType = Int64
    public typealias SelfType = UInt
    
    public init?(withTypedValue value: Int64?) {
        guard value != nil else {
            return nil
        }
        self = UInt(UInt64(bitPattern: value!))
    }
    
    public func archivedTypedValue() -> Int64? {
        return Int64(self)
    }    
}

extension Int8 : CodableStructColumn {
    public typealias FundamentalType = Int32
    public typealias SelfType = Int8
    
    public init?(withTypedValue value: Int32?) {
        guard value != nil else {
            return nil
        }
        self = Int8(value!)
    }
    
    public func archivedTypedValue() -> Int32? {
        return Int32(self)
    }    
}

extension Int16 : CodableStructColumn {
    public typealias FundamentalType = Int32
    public typealias SelfType = Int16
    
    public init?(withTypedValue value: Int32?) {
        guard value != nil else {
            return nil
        }
        self = Int16(value!)
    }
    
    public func archivedTypedValue() -> Int32? {
        return Int32(self)
    }    
}

extension UInt8 : CodableStructColumn {
    public typealias FundamentalType = Int32
    public typealias SelfType = UInt8
    
    public init?(withTypedValue value: Int32?) {
        guard value != nil else {
            return nil
        }
        self = UInt8(value!)
    }
    
    public func archivedTypedValue() -> Int32? {
        return Int32(self)
    }    
}

extension UInt16 : CodableStructColumn {
    public typealias FundamentalType = Int32
    public typealias SelfType = UInt16
    
    public init?(withTypedValue value: Int32?) {
        guard value != nil else {
            return nil
        }
        self = UInt16(value!)
    }
    
    public func archivedTypedValue() -> Int32? {
        return Int32(self)
    }    
}

extension UInt32 : CodableStructColumn {
    public typealias FundamentalType = Int32
    public typealias SelfType = UInt32
    
    public init?(withTypedValue value: Int32?) {
        guard value != nil else {
            return nil
        }
        self = UInt32(bitPattern: value!)
    }
    
    public func archivedTypedValue() -> Int32? {
        return Int32(self)
    }    
}

extension UInt64 : CodableStructColumn {
    public typealias FundamentalType = Int64
    public typealias SelfType = UInt64
    
    public init?(withTypedValue value: Int64?) {
        guard value != nil else {
            return nil
        }
        self = UInt64(bitPattern: value!)
    }
    
    public func archivedTypedValue() -> Int64? {
        return Int64(self)
    }    
}

extension Float: CodableStructColumn {
    public typealias FundamentalType = Double
    public typealias SelfType = Float
    
    public func archivedTypedValue() -> Double? {
        return Double(self)
    }
    
    public init?(withTypedValue value: Double?) {
        guard value != nil else {
            return nil
        }
        self = Float(value!)
    }
}

extension Bool : CodableStructColumn {
    public typealias FundamentalType = Int32
    public typealias SelfType = Bool
    
    public init?(withTypedValue value: Int32?) {
        guard value != nil else {
            return nil
        }
        self = value != 0
    }
    
    public func archivedTypedValue() -> Int32? {
        return self ? 1 : 0
    }    
}

extension Date: CodableStructColumn {
    public typealias FundamentalType = Int64
    
    public init?(withTypedValue optionalValue: Int64?) {
        guard let value = optionalValue else {
            return nil
        }
        self.init(timeIntervalSince1970: TimeInterval(value))
    }
    
    public func archivedTypedValue() -> Int64? {
        return Int64(self.timeIntervalSince1970)
    }
}

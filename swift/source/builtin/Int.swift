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

extension Int : ColumnCoding {
    public typealias FundamentalType = Int64
    public typealias SelfType = Int
    
    public init?(with value: FundamentalType?) {
        guard value != nil else {
            return nil
        }
        self = SelfType(value!)
    }

    public func archivedValue() -> FundamentalType? {
        return FundamentalType(self)
    }    
}

extension Int8 : ColumnCoding {
    public typealias FundamentalType = Int32
    public typealias SelfType = Int8
    
    public init?(with value: FundamentalType?) {
        guard value != nil else {
            return nil
        }
        self = SelfType(value!)
    }
    
    public func archivedValue() -> FundamentalType? {
        return FundamentalType(self)
    }    
}

extension Int16 : ColumnCoding {
    public typealias FundamentalType = Int32
    public typealias SelfType = Int16
    
    public init?(with value: FundamentalType?) {
        guard value != nil else {
            return nil
        }
        self = SelfType(value!)
    }
    
    public func archivedValue() -> FundamentalType? {
        return FundamentalType(self)
    }    
}

extension Int32 : FundamentalCoding {}

extension Int64 : FundamentalCoding {}

extension UInt : ColumnCoding {
    public typealias FundamentalType = Int32
    public typealias SelfType = UInt
    
    public init?(with value: FundamentalType?) {
        guard value != nil else {
            return nil
        }
        self = SelfType(value!)
    }
    
    public func archivedValue() -> FundamentalType? {
        return FundamentalType(self)
    }    
}

extension UInt8 : ColumnCoding {
    public typealias FundamentalType = Int32
    public typealias SelfType = UInt8
    
    public init?(with value: FundamentalType?) {
        guard value != nil else {
            return nil
        }
        self = SelfType(value!)
    }
    
    public func archivedValue() -> FundamentalType? {
        return FundamentalType(self)
    }    
}

extension UInt16 : ColumnCoding {
    public typealias FundamentalType = Int32
    public typealias SelfType = UInt16
    
    public init?(with value: FundamentalType?) {
        guard value != nil else {
            return nil
        }
        self = SelfType(value!)
    }
    
    public func archivedValue() -> FundamentalType? {
        return FundamentalType(self)
    }    
}

extension UInt32 : ColumnCoding {
    public typealias FundamentalType = Int32
    public typealias SelfType = UInt32
    
    public init?(with value: FundamentalType?) {
        guard value != nil else {
            return nil
        }
        self = SelfType(value!)
    }
    
    public func archivedValue() -> FundamentalType? {
        return FundamentalType(self)
    }    
}

extension UInt64 : ColumnCoding {
    public typealias FundamentalType = Int64
    public typealias SelfType = UInt64
    
    public init?(with value: FundamentalType?) {
        guard value != nil else {
            return nil
        }
        self = SelfType(value!)
    }
    
    public func archivedValue() -> FundamentalType? {
        return FundamentalType(self)
    }    
}


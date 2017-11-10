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

public protocol ColumnCodingBase {
    static var columnType: ColumnType {get}
    
    func archivedValue() -> FundamentalCodingBase?
    
    init?(with value: FundamentalCodingBase?)
}

extension ColumnCodingBase {
    public static var columnType: ColumnType {
        fatalError()
    }
}

public protocol ColumnCoding: ColumnCodingBase, LiteralValueConvertible {
    associatedtype FundamentalType   // Int32 Int64 String Double Data
    
    init?(with value: FundamentalType?)
    
    func archivedValue() -> FundamentalType?
    
    init() //FIXME: remove it before Beta release
}

extension ColumnCoding {
    public init?(with value: FundamentalCodingBase?) {
        self.init(with: value as? FundamentalType)
    }
    
    public func archivedValue() -> FundamentalCodingBase? {
        let value: FundamentalType? = archivedValue()
        return value as? FundamentalCodingBase
    }
    
    public func asLiteralValue() -> LiteralValue {
        return LiteralValue(self)
    }
}

extension ColumnCoding where FundamentalType==Int32 {
    public static var columnType: ColumnType {
        return ColumnType.Integer32
    }
}

extension ColumnCoding where FundamentalType==Int64 {
    public static var columnType: ColumnType {
        return ColumnType.Integer64
    }
}

extension ColumnCoding where FundamentalType==String {
    public static var columnType: ColumnType {
        return ColumnType.Text
    }
}

extension ColumnCoding where FundamentalType==Double {
    public static var columnType: ColumnType {
        return ColumnType.Float
    }
}

extension ColumnCoding where FundamentalType==Data {
    public static var columnType: ColumnType {
        return ColumnType.BLOB
    }
}

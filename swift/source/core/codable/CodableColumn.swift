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

public protocol CodableColumnBase {
    static var columnType: ColumnType {get}
    
    init?(withFundamentalValue value: FundamentalValue)
    
    func archivedFundamentalValue() -> FundamentalValue
}

//TODO: Maybe I should use Codable protocol to implement this ?
public protocol CodableColumn: CodableColumnBase, LiteralValueConvertible {
    associatedtype FundamentalType   // Int32 Int64 String Double Data
    
    init?(withTypedValue value: FundamentalType?)
    
    func archivedTypedValue() -> FundamentalType?
}

extension CodableColumn {
    public init?(withFundamentalValue value: FundamentalValue) {
        self.init(withTypedValue: (value as? FundamentalType))
    }
    
    public func archivedFundamentalValue() -> FundamentalValue {
        return archivedTypedValue() as? CodableFundamentalValueBase 
    }
    
    public func asLiteralValue() -> LiteralValue {
        return LiteralValue(self)
    }
}

extension CodableColumn {
    public static var columnType: ColumnType {
        Error.abort("")
    }
}

extension CodableColumn where FundamentalType==Int32 {
    public static var columnType: ColumnType {
        return ColumnType.Integer32
    }
}

extension CodableColumn where FundamentalType==Int64 {
    public static var columnType: ColumnType {
        return ColumnType.Integer64
    }
}

extension CodableColumn where FundamentalType==String {
    public static var columnType: ColumnType {
        return ColumnType.Text
    }
}

extension CodableColumn where FundamentalType==Double {
    public static var columnType: ColumnType {
        return ColumnType.Float
    }
}

extension CodableColumn where FundamentalType==Data {
    public static var columnType: ColumnType {
        return ColumnType.BLOB
    }
}

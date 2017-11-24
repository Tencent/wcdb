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

public protocol CodableFundamentalValueBase {}

//Int32, Int64, Double, String, Data
public protocol CodableFundamentalValue: CodableStructColumn, CodableFundamentalValueBase {}

extension CodableFundamentalValue {
    public typealias FundamentalType = Self
    
    public init?(withTypedValue value: FundamentalType?) {
        guard value != nil else {
            return nil
        }
        self = value!
    }
    
    public func archivedTypedValue() -> FundamentalType? {
        return self
    }
}


extension CodableColumn where FundamentalType==Int32 {
    public static var columnType: ColumnType {
        return ColumnType.Integer32
    }
}
extension Int32 : CodableFundamentalValue {}

extension CodableColumn where FundamentalType==Int64 {
    public static var columnType: ColumnType {
        return ColumnType.Integer64
    }
}
extension Int64 : CodableFundamentalValue {}

extension CodableColumn where FundamentalType==Double {
    public static var columnType: ColumnType {
        return ColumnType.Float
    }
}
extension Double : CodableFundamentalValue {}

extension CodableColumn where FundamentalType==String {
    public static var columnType: ColumnType {
        return ColumnType.Text
    }
}
extension String: CodableFundamentalValue {}

extension CodableColumn where FundamentalType==Data {
    public static var columnType: ColumnType {
        return ColumnType.BLOB
    }
}
extension Data: CodableFundamentalValue {}


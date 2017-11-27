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

// nullable, Int32, Int64, Double, String, Data
public class FundamentalValue: ExpressibleByNilLiteral, ExpressibleByIntegerLiteral, ExpressibleByBooleanLiteral, ExpressibleByFloatLiteral, ExpressibleByStringLiteral {
    let base: CodableFundamentalValueBase?
    
    init(with codableFundamentalValueBase: CodableFundamentalValueBase?) {
        base = codableFundamentalValueBase
    }
    
    init(_ codableFundamentalValueBase: CodableFundamentalValueBase?) {
        base = codableFundamentalValueBase
    }
    
    public convenience required init(nilLiteral: ()) {
        self.init(nil)
    }

    public convenience required init(integerLiteral value: Int) {
        self.init(Int64(value))
    }

    public convenience required init(booleanLiteral value: Bool) {
        self.init(value ? 1 : 0)
    }

    public convenience required init(floatLiteral value: Double) {
        self.init(value)
    }

    public convenience required init(stringLiteral value: String) {
        self.init(value)
    }
        
    public var type: ColumnType {
        guard base != nil else {
            return .Null
        }
        return Swift.type(of: base!).columnType
    }
    
    public var int32Value: Int32 {
        return base as? Int32 ?? 0
    }
    
    public var int64Value: Int64 {
        return base as? Int64 ?? 0
    }

    public var stringValue: String {
        return base as? String ?? ""
    }

    public var doubleValue: Double {
        return base as? Double ?? 0
    }

    public var dataValue: Data {
        return base as? Data ?? Data()
    }
}

public typealias FundamentalColumn = [FundamentalValue]
public typealias FundamentalRow = [FundamentalValue]

public typealias FundamentalRowXColumn = [FundamentalRow]

extension Array where Element==Array<FundamentalValue> {
    public subscript(row row: Array.Index, column column: Array<FundamentalValue>.Index) -> FundamentalValue {
        return self[row][column]
    }
    
    public subscript(row row: Array.Index) -> FundamentalColumn {
        return self[row]
    }
    
    public func forEach(_ body: (_ row: Array.Index, _ column: Array<FundamentalValue>.Index, _ value: FundamentalValue) throws -> Void) rethrows {
        for (rowIndex, row) in self.enumerated() {
            for (columnIndex, value) in row.enumerated() {
                try body(rowIndex, columnIndex, value)
            }
        }
    }
}


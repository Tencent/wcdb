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
public class FundamentalValue: ExpressibleByNilLiteral, ExpressibleByIntegerLiteral, ExpressibleByBooleanLiteral, ExpressibleByFloatLiteral, ExpressibleByStringLiteral, CustomDebugStringConvertible {
    let base: FundamentalColumnType?
    
    init(_ _: Void? = nil) {
        base = nil
    }
    
    init<ColumnEncodableType: ColumnEncodable>(_ columnEncodableValue: ColumnEncodableType) {
        base = columnEncodableValue.archivedValue()
    }
    
    public convenience required init(nilLiteral: ()) {
        self.init(nil)
    }
    
    public convenience required init(integerLiteral value: Int) {
        self.init(value)
    }
    
    public convenience required init(booleanLiteral value: Bool) {
        self.init(value)
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
        switch type {
        case .Integer32:
            return base as! Int32
        case .Integer64:
            let int64Value = base as! Int64
            return Int32(truncatingIfNeeded: int64Value)
        case .Float:
            let doubleValue = base as! Double
            return Int32(doubleValue)
        case .Text:
            let stringValue = base as! String
            return Int32(stringValue) ?? 0
        default:
            return 0
        }
    }
    
    public var int64Value: Int64 {
        switch type {
        case .Integer32:
            let int32Value = base as! Int32
            return Int64(int32Value)
        case .Integer64:
            return base as! Int64
        case .Float:
            let doubleValue = base as! Double
            return Int64(doubleValue)
        case .Text:
            let stringValue = base as! String
            return Int64(stringValue) ?? 0
        default:
            return 0
        }
    }
    
    public var stringValue: String {
        switch type {
        case .Integer32:
            let int32Value = base as! Int32
            return "\(int32Value)"
        case .Integer64:
            let int64Value = base as! Int64
            return "\(int64Value)"
        case .Float:
            let doubleValue = base as! Double
            return "\(doubleValue)"
        case .Text:
            return base as! String
        case .BLOB:
            let dataValue = base as! Data
            return String(data: dataValue, encoding: .utf8) ?? ""
        default:
            return ""
        }
    }
    
    public var doubleValue: Double {
        switch type {
        case .Integer32:
            let int32Value = base as! Int32
            return Double(int32Value)
        case .Integer64:
            let int64Value = base as! Int64
            return Double(int64Value)
        case .Float:
            return base as! Double
        case .Text:
            let stringValue = base as! String
            return Double(stringValue) ?? 0
        default:
            return 0
        }
    }
    
    public var dataValue: Data {
        switch type {
        case .Integer32:
            fallthrough
        case .Integer64:
            fallthrough
        case .Float:
            fallthrough
        case .Text:
            let stringValue = base as! String
            return stringValue.data(using: .utf8) ?? Data()
        case .BLOB:
            return base as! Data
        default:
            return Data()
        }
    }
    
    public var debugDescription: String {
        switch type {
        case .Null:
            return "NULL"
        default:
            return stringValue
        }
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



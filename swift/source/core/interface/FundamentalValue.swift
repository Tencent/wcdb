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
public final class FundamentalValue {
    let base: FundamentalColumnType?

    init(_ _: Void? = nil) {
        base = nil
    }

    init<ColumnEncodableType: ColumnEncodable>(_ columnEncodableValue: ColumnEncodableType) {
        base = columnEncodableValue.archivedValue()
    }

    public var type: ColumnType {
        guard base != nil else {
            return .null
        }
        return Swift.type(of: base!).columnType
    }

    public var int32Value: Int32 {
        switch type {
        case .integer32:
            return (base as? Int32) ?? 0
        case .integer64:
            let int64Value = (base as? Int64) ?? 0
            return Int32(truncatingIfNeeded: int64Value)
        case .float:
            let doubleValue = (base as? Double) ?? 0
            return Int32(doubleValue)
        case .text:
            let stringValue = (base as? String) ?? ""
            return Int32(stringValue) ?? 0
        default:
            return 0
        }
    }

    public var int64Value: Int64 {
        switch type {
        case .integer32:
            let int32Value = (base as? Int32) ?? 0
            return Int64(int32Value)
        case .integer64:
            return (base as? Int64) ?? 0
        case .float:
            let doubleValue = (base as? Double) ?? 0
            return Int64(doubleValue)
        case .text:
            let stringValue = (base as? String) ?? ""
            return Int64(stringValue) ?? 0
        default:
            return 0
        }
    }

    public var stringValue: String {
        switch type {
        case .integer32:
            guard let int32Value = (base as? Int32) else {
                return ""
            }
            return "\(int32Value)"
        case .integer64:
            guard let int64Value = (base as? Int64) else {
                return ""
            }
            return "\(int64Value)"
        case .float:
            guard let doubleValue = (base as? Double) else {
                return ""
            }
            return "\(doubleValue)"
        case .text:
            return (base as? String) ?? ""
        case .BLOB:
            guard let dataValue = (base as? Data), let stringValue = String(data: dataValue, encoding: .utf8) else {
                return ""
            }
            return stringValue
        default:
            return ""
        }
    }

    public var doubleValue: Double {
        switch type {
        case .integer32:
            let int32Value = (base as? Int32) ?? 0
            return Double(int32Value)
        case .integer64:
            let int64Value = (base as? Int64) ?? 0
            return Double(int64Value)
        case .float:
            return (base as? Double) ?? 0
        case .text:
            guard let stringValue = base as? String else {
                return 0
            }
            return Double(stringValue) ?? 0
        default:
            return 0
        }
    }

    public var dataValue: Data {
        switch type {
        case .integer32:
            fallthrough
        case .integer64:
            fallthrough
        case .float:
            fallthrough
        case .text:
            guard let stringValue = base as? String, let dataValue = stringValue.data(using: .utf8) else {
                return Data()
            }
            return dataValue
        case .BLOB:
            return (base as? Data) ?? Data()
        default:
            return Data()
        }
    }
}

extension FundamentalValue: ExpressibleByNilLiteral {
    public convenience init(nilLiteral: ()) {
        self.init(nil)
    }
}

extension FundamentalValue: ExpressibleByIntegerLiteral {
    public convenience init(integerLiteral value: Int) {
        self.init(value)
    }
}

extension FundamentalValue: ExpressibleByBooleanLiteral {
    public convenience init(booleanLiteral value: Bool) {
        self.init(value)
    }
}

extension FundamentalValue: ExpressibleByFloatLiteral {
    public convenience init(floatLiteral value: Double) {
        self.init(value)
    }
}

extension FundamentalValue: ExpressibleByStringLiteral {
    public convenience init(stringLiteral value: String) {
        self.init(value)
    }
}

extension FundamentalValue: CustomDebugStringConvertible {
    public var debugDescription: String {
        switch type {
        case .null:
            return "NULL"
        default:
            return stringValue
        }
    }
}

public typealias FundamentalColumn = [FundamentalValue]
public typealias FundamentalRow = [FundamentalValue]

public typealias FundamentalRowXColumn = [FundamentalRow]

extension Array where Element==[FundamentalValue] {
    public subscript(row row: Array.Index, column column: Array<FundamentalValue>.Index) -> FundamentalValue {
        return self[row][column]
    }

    public subscript(row row: Array.Index) -> FundamentalColumn {
        return self[row]
    }

    public typealias FundamentalIteration = (
        _ row: Array.Index,
        _ column: Array<FundamentalValue>.Index,
        _ value: FundamentalValue) throws -> Void

    public func forEach(_ body: FundamentalIteration) rethrows {
        for (rowIndex, row) in self.enumerated() {
            for (columnIndex, value) in row.enumerated() {
                try body(rowIndex, columnIndex, value)
            }
        }
    }
}

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
public struct FundamentalValue {
    private let base: Any?
    public let type: ColumnType

    public init(_ _: Void? = nil) {
        base = nil
        type = .null
    }

    public init(_ value: Int32) {
        base = value
        type = .integer32
    }

    public init(_ value: Int64) {
        base = value
        type = .integer64
    }

    public init(_ value: Double) {
        base = value
        type = .float
    }

    public init(_ value: String) {
        base = value
        type = .text
    }

    public init(_ value: Data) {
        base = value
        type = .BLOB
    }

    public init<T: ColumnEncodable>(_ encodedValue: T) {
        self = encodedValue.archivedValue()
    }

    public var int32Value: Int32 {
        switch type {
        case .integer32:
            return base as! Int32
        case .integer64:
            return Int32(truncatingIfNeeded: base as! Int64)
        case .float:
            return Int32(base as! Double)
        case .text:
            return Int32(base as! String) ?? 0
        default:
            return 0
        }
    }

    public var int64Value: Int64 {
        switch type {
        case .integer32:
            return Int64(base as! Int32)
        case .integer64:
            return base as! Int64
        case .float:
            return Int64(base as! Double)
        case .text:
            return Int64(base as! String) ?? 0
        default:
            return 0
        }
    }

    public var stringValue: String {
        switch type {
        case .integer32:
            return "\(base as! Int32)"
        case .integer64:
            return "\(base as! Int64)"
        case .float:
            return "\(base as! Double)"
        case .text:
            return base as! String
        case .BLOB:
            return String(data: base as! Data, encoding: .utf8) ?? ""
        default:
            return ""
        }
    }

    public var doubleValue: Double {
        switch type {
        case .integer32:
            return Double(base as! Int32)
        case .integer64:
            return Double(base as! Int64)
        case .float:
            return base as! Double
        case .text:
            return Double(base as! String) ?? 0
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
            return stringValue.data(using: .utf8) ?? Data()
        case .BLOB:
            return (base as? Data) ?? Data()
        default:
            return Data()
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
}

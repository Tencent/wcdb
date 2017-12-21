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
public struct LiteralValue: Describable {
    public private(set) var description: String

    public init(_ value: Int32) {
        description = String(value)
    }

    public init(_ value: Int64) {
        description = String(value)
    }

    public init(_ value: Bool) {
        description = String(value ? 1 : 0)
    }

    public init(_ value: Double) {
        description = String(value)
    }

    public init(_ value: String) {
        description = "'\(value.replacingOccurrences(of: "'", with: "''"))'"
    }

    public init(_ value: Data) {
        let string = String(data: value, encoding: .utf8) ?? ""
        description = "'\(string.replacingOccurrences(of: "'", with: "''"))'"
    }

    public init(_ value: Void?) {
        description = "NULL"
    }

    public init<ColumnEncodableType: ColumnEncodableBase>(_ columnEncodableValue: ColumnEncodableType) {
        if let value = columnEncodableValue.archivedFundamentalValue() {
            switch ColumnEncodableType.columnType {
            case .integer32:
                let int32Value = value as? Int32
                Error.assert(int32Value != nil, message: "Conversion error. Please report an issue to us.")
                self.init(int32Value!)
            case .integer64:
                let int64Value = value as? Int64
                Error.assert(int64Value != nil, message: "Conversion error. Please report an issue to us.")
                self.init(int64Value!)
            case .float:
                let doubleValue = value as? Double
                Error.assert(doubleValue != nil, message: "Conversion error. Please report an issue to us.")
                self.init(doubleValue!)
            case .text:
                let stringValue = value as? String
                Error.assert(stringValue != nil, message: "Conversion error. Please report an issue to us.")
                self.init(stringValue!)
            case .BLOB:
                let dataValue = value as? Data
                Error.assert(dataValue != nil, message: "Conversion error. Please report an issue to us.")
                self.init(dataValue!)
            case .null:
                self.init(nil)
            }
        } else {
            self.init(nil)
        }
    }
}

extension LiteralValue: ExpressibleByNilLiteral {
    public init(nilLiteral: ()) {
        self.init(nilLiteral)
    }
}

extension LiteralValue: ExpressibleByIntegerLiteral {
    public init(integerLiteral value: Int) {
        self.init(value)
    }
}

extension LiteralValue: ExpressibleByBooleanLiteral {
    public init(booleanLiteral value: Bool) {
        self.init(value)
    }
}

extension LiteralValue: ExpressibleByFloatLiteral {
    public init(floatLiteral value: Double) {
        self.init(value)
    }
}

extension LiteralValue: ExpressibleByStringLiteral {
    public init(stringLiteral value: String) {
        self.init(value)
    }
}

extension LiteralValue: LiteralValueConvertible {
    public func asLiteralValue() -> LiteralValue {
        return self
    }
}

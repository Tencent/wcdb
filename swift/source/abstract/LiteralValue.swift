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

    public init<ColumnEncodableType: ColumnEncodable>(_ columnEncodableValue: ColumnEncodableType) {
        if let value = columnEncodableValue.archivedFundamentalValue() {
            switch ColumnEncodableType.columnType {
            case .integer32:
                guard let int32Value = value as? Int32 else {
                    Error.abort("")
                }
                self.init(int32Value)
            case .integer64:
                guard let int64Value = value as? Int64 else {
                    Error.abort("")
                }
                self.init(int64Value)
            case .float:
                guard let doubleValue = value as? Double else {
                    Error.abort("")
                }
                self.init(doubleValue)
            case .text:
                guard let stringValue = value as? String else {
                    Error.abort("")
                }
                self.init(stringValue)
            case .BLOB:
                guard let dataValue = value as? Data else {
                    Error.abort("")
                }
                self.init(dataValue)
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

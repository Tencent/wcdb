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
import WCDB_Private

public final class LiteralValue: Identifier<CPPLiteralValue> {

    public convenience init(_ value: Int32) {
        self.init(with: WCDBLiteralValueCreateWithInt64(Int64(value)))
    }

    public convenience init(_ value: Int64) {
        self.init(with: WCDBLiteralValueCreateWithInt64(value))
    }

    public convenience init(_ value: Bool) {
        self.init(with: WCDBLiteralValueCreateWithBool(value))
    }

    public convenience init(_ value: Double) {
        self.init(with: WCDBLiteralValueCreateWithDouble(value))
    }

    public convenience init(_ value: String) {
        self.init(with: WCDBLiteralValueCreateWithString(value.cString))
    }

    public convenience init(_ value: Data) {
        let string = String(data: value, encoding: .utf8) ?? ""
        self.init(with: WCDBLiteralValueCreateWithString(string.cString))
    }

    public convenience init(_ value: Void?) {
        self.init(with: WCDBLiteralValueCreateWithNull())
    }

    public convenience init(_ value: Value) {
        switch value.type {
        case .integer32:
            self.init(value.int32Value)
        case .integer64:
            self.init(value.int64Value)
        case .float:
            self.init(value.doubleValue)
        case .text:
            self.init(value.stringValue)
        case .BLOB:
            self.init(value.dataValue)
        case .null:
            self.init(nil)
        }
    }

    public convenience init<T: ColumnEncodable>(_ encodedValue: T) {
        self.init(encodedValue.archivedValue())
    }

    public static func currentTime() -> LiteralValue {
        LiteralValue(with: WCDBLiteralValueCreateWithCurrentTime())
    }

    public static func currentDate() -> LiteralValue {
        LiteralValue(with: WCDBLiteralValueCreateWithCurrentDate())
    }

    public static func currentTimestamp() -> LiteralValue {
        LiteralValue(with: WCDBLiteralValueCreateWithCurrentTimestamp())
    }
}

extension LiteralValue: ExpressibleByNilLiteral {
    public convenience init(nilLiteral: ()) {
        self.init(nilLiteral)
    }
}

extension LiteralValue: ExpressibleByIntegerLiteral {
    public convenience init(integerLiteral value: Int) {
        self.init(value)
    }
}

extension LiteralValue: ExpressibleByBooleanLiteral {
    public convenience init(booleanLiteral value: Bool) {
        self.init(value)
    }
}

extension LiteralValue: ExpressibleByFloatLiteral {
    public convenience init(floatLiteral value: Double) {
        self.init(value)
    }
}

extension LiteralValue: ExpressibleByStringLiteral {
    public convenience init(stringLiteral value: String) {
        self.init(value)
    }
}

extension LiteralValue: LiteralValueConvertible {
    public func asLiteralValue() -> LiteralValue {
        return self
    }
}

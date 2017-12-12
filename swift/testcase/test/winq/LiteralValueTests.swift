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

import XCTest
import WCDBSwift

class LiteralValueTests: BaseTestCase {

    struct Int32LiteralValue: ColumnCodable {
        func archivedValue() -> Int32? {
            return int32
        }
        init?(with value: Int32) {
            self.init(value)
        }
        init(_ value: Int32) {
            int32 = value
        }
        typealias FundamentalType = Int32
        var int32: Int32
    }

    struct Int64LiteralValue: ColumnCodable {
        func archivedValue() -> Int64? {
            return int64
        }
        init?(with value: Int64) {
            self.init(value)
        }
        init(_ value: Int64) {
            int64 = value
        }
        typealias FundamentalType = Int64
        var int64: Int64
    }

    struct FloatLiteralValue: ColumnCodable {
        func archivedValue() -> Double? {
            return double
        }
        init?(with value: Double) {
            self.init(value)
        }
        init(_ value: Double) {
            double = value
        }
        typealias FundamentalType = Double
        var double: Double
    }

    struct StringLiteralValue: ColumnCodable {
        func archivedValue() -> String? {
            return string
        }
        init?(with value: String) {
            self.init(value)
        }
        init(_ value: String) {
            string = value
        }
        typealias FundamentalType = String
        var string: String
    }

    struct DataLiteralValue: ColumnCodable {
        func archivedValue() -> Data? {
            return data
        }
        init?(with value: Data) {
            self.init(value)
        }
        init(_ value: Data) {
            data = value
        }
        typealias FundamentalType = Data
        var data: Data
    }

    func testLiteralValue() {

        let boolLiteralValue: LiteralValue = false
        WINQAssertEqual(boolLiteralValue, "0")

        let intLiteralValue: LiteralValue = 1
        WINQAssertEqual(intLiteralValue, "1")

        let floatLiteralValue: LiteralValue = 0.1
        WINQAssertEqual(floatLiteralValue, "0.1")

        let stringLiteralValue: LiteralValue = "test"
        WINQAssertEqual(stringLiteralValue, "'test'")

        let nullLiteralValue: LiteralValue = nil
        WINQAssertEqual(nullLiteralValue, "NULL")

        WINQAssertEqual(LiteralValue(true), "1")

        WINQAssertEqual(LiteralValue(1), "1")

        WINQAssertEqual(LiteralValue(17626545782784), "17626545782784")

        WINQAssertEqual(LiteralValue(0.1), "0.1")

        WINQAssertEqual(LiteralValue("test"), "'test'")

        WINQAssertEqual(LiteralValue("test".data(using: .ascii)!), "'test'")

        WINQAssertEqual(LiteralValue(nil), "NULL")

        WINQAssertEqual(LiteralValue(Int32LiteralValue(1)), "1")

        WINQAssertEqual(LiteralValue(Int64LiteralValue(17626545782784)), "17626545782784")

        WINQAssertEqual(LiteralValue(FloatLiteralValue(0.1)), "0.1")

        WINQAssertEqual(LiteralValue(StringLiteralValue("test")), "'test'")

        WINQAssertEqual(LiteralValue(DataLiteralValue("test".data(using: .ascii)!)), "'test'")
    }
}

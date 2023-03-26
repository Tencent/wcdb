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
#if TEST_WCDB_SWIFT
import WCDBSwift
#else
import WCDB
#endif

class TestObjectBase: Named, Hashable, Comparable, CustomDebugStringConvertible {
    func hash(into hasher: inout Hasher) {
        fatalError()
    }

    var isAutoIncrement: Bool = false
    var lastInsertedRowID: Int64 = 0

    func isSame(_ other: TestObjectBase) -> Bool {
        fatalError()
    }

    func isLessThan(_ other: TestObjectBase) -> Bool {
        fatalError()
    }

    static func == (lhs: TestObjectBase, rhs: TestObjectBase) -> Bool {
        return lhs.hashValue == rhs.hashValue
    }

    static func < (lhs: TestObjectBase, rhs: TestObjectBase) -> Bool {
        return lhs.hashValue < rhs.hashValue
    }

    var debugDescription: String {
        fatalError()
    }
}

final class TestObject: TestObjectBase, TableCodable {
    var variable1: Int? = 0
    var variable2: String? = ""

    enum CodingKeys: String, CodingTableKey {
        typealias Root = TestObject
        case variable1
        case variable2
        static let objectRelationalMapping = TableBinding(CodingKeys.self) {
            BindColumnConstraint(variable1, isPrimary: true, orderBy: .ascending, isAutoIncrement: true)
        }
    }

    static func == (lhs: TestObject, rhs: TestObject) -> Bool {
        return lhs.variable1 == rhs.variable1 && lhs.variable2 == rhs.variable2
    }

    static func < (lhs: TestObject, rhs: TestObject) -> Bool {
        if lhs.variable1 == rhs.variable1 {
            if let lhsv2 = lhs.variable2, let rhsv2 = rhs.variable2 {
                return lhsv2 < rhsv2
            }
            return lhs.variable2 == nil
        }
        if let lhsv1 = lhs.variable1, let rhsv1 = rhs.variable1 {
            return lhsv1 < rhsv1
        }
        return lhs.variable1 == nil
    }

    override var hashValue: Int {
        return (String(variable1 ?? 0)+(variable2 ?? "")).hashValue
    }

    override var debugDescription: String {
        return "\(variable1 != nil ? String(variable1!) : "nil"), \(variable2 ?? "nil")"
    }
}

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
import WCDBSwift

class CRUDObjectBase: Named, Hashable, Comparable, CustomDebugStringConvertible {
    var hashValue: Int {
        fatalError()
    }

    var isAutoIncrement: Bool = false
    var lastInsertedRowID: Int64 = 0

    func isSame(_ other: CRUDObjectBase) -> Bool {
        fatalError()
    }

    func isLessThan(_ other: CRUDObjectBase) -> Bool {
        fatalError()
    }

    static func == (lhs: CRUDObjectBase, rhs: CRUDObjectBase) -> Bool {
        return lhs.hashValue == rhs.hashValue
    }

    static func < (lhs: CRUDObjectBase, rhs: CRUDObjectBase) -> Bool {
        return lhs.hashValue < rhs.hashValue
    }

    var debugDescription: String {
        fatalError()
    }
}

class CRUDObject: CRUDObjectBase, TableCodable {
    var variable1: Int? = 0
    var variable2: String? = ""

    enum CodingKeys: String, CodingTableKey {
        typealias Root = CRUDObject
        case variable1
        case variable2
        static let objectRelationalMapping = TableBinding(CodingKeys.self)
        static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
            return [.variable1: ColumnConstraintBinding(isPrimary: true, orderBy: .ascending, isAutoIncrement: true)]
        }
    }

    static func == (lhs: CRUDObject, rhs: CRUDObject) -> Bool {
        return lhs.variable1 == rhs.variable1 && lhs.variable2 == rhs.variable2
    }

    static func < (lhs: CRUDObject, rhs: CRUDObject) -> Bool {
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

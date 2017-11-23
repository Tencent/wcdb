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

class Sequence: WCDB.CodableTable, CustomStringConvertible {
    public static let tableName: String = "sqlite_sequence"
    
    public var name: String? = nil
    public var seq: Int? = nil
    
    public required init() {}
    
    public var description: String {
        return "name:\(name ?? ""), seq:\(seq ?? Int.max)"
    }

    //WCDB
    public static var objectRelationalMapping: TableBinding = TableBinding(Sequence.self)
}

//WCDB
extension WCDB.Sequence {
    public static func columnBindings() -> [AnyColumnBinding] {
        return [
            ColumnBinding(\Sequence.name, alias: "name"),
            ColumnBinding(\Sequence.seq, alias: "seq")]
    }
}

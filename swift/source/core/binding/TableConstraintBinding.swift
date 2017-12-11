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

public class TableConstraintBinding: AnyBinding {
    public typealias Name = String

    let conflict: Conflict?
    let columnIndexes: [ColumnIndex]
    let condition: Condition?

    public convenience init(indexesBy columnIndexConvertibleList: ColumnIndexConvertible...,
                            onConflict conflict: Conflict? = nil,
                            check condition: Condition? = nil) {
        self.init(indexesBy: columnIndexConvertibleList, onConflict: conflict, check: condition)
    }

    public init(indexesBy columnIndexConvertibleList: [ColumnIndexConvertible],
                onConflict conflict: Conflict? = nil,
                check condition: Condition? = nil) {
        self.columnIndexes = columnIndexConvertibleList.asIndexes()
        self.conflict = conflict
        self.condition = condition
        super.init(with: .tableConstraint)
    }

    func generateConstraint(withName name: String) -> TableConstraint {
        Error.abort("")
    }
}

public class MultiPrimaryBinding: TableConstraintBinding {
    override func generateConstraint(withName name: String) -> TableConstraint {
        var tableConstraint = TableConstraint(named: name).makePrimary(indexesBy: columnIndexes)
        if conflict != nil {
            tableConstraint = tableConstraint.onConflict(conflict!)
        }
        if condition != nil {
            tableConstraint = tableConstraint.check(condition!)
        }
        return tableConstraint
    }
}

public class MultiUniqueBinding: TableConstraintBinding {
    override func generateConstraint(withName name: String) -> TableConstraint {
        var tableConstraint = TableConstraint(named: name).makeUnique(indexesBy: columnIndexes)
        if conflict != nil {
            tableConstraint = tableConstraint.onConflict(conflict!)
        }
        if condition != nil {
            tableConstraint = tableConstraint.check(condition!)
        }
        return tableConstraint
    }
}

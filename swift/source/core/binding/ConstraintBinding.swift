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
public class ConstraintBinding: AnyBinding {
    let name: String
    let conflict: Conflict?
    let columnIndexes: [ColumnIndex]
    let condition: Condition?
    
    init(with type: AnyBinding.BindingType, named name: String, indexesBy columnIndexConvertibleList: [ColumnIndexConvertible], onConflict conflict: Conflict? = nil, check condition: Condition? = nil) {
        self.name = name
        self.columnIndexes = columnIndexConvertibleList.asIndexes()
        self.conflict = conflict
        self.condition = condition
        super.init(with: type)
    }
    
    func generateConstraint() -> TableConstraint {
        fatalError()
    }
}

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
public class IndexBinding: AnyBinding {
    let columnIndexes: [ColumnIndex]
    let subfix: String 
    let isUnique: Bool
    
    public init(withSubfix subfix: String, isUnique: Bool = false, indexesBy columnIndexConvertibleList: [ColumnIndexConvertible]) {
        self.subfix = subfix
        self.columnIndexes = columnIndexConvertibleList.asIndexes()
        self.isUnique = isUnique
        super.init(with: .Index)
    }
    
    public convenience init(withSubfix subfix: String, isUnique: Bool = false, indexesBy columnIndexConvertibleList: ColumnIndexConvertible...) {
        self.init(withSubfix: subfix, isUnique: isUnique, indexesBy: columnIndexConvertibleList)
    }
    
    func generateCreateIndexStatement(prefix tableName: String) -> StatementCreateIndex {
        return StatementCreateIndex().create(index: tableName+subfix, isUnique: isUnique, ifNotExists: true).on(table: tableName, indexesBy: columnIndexes)
    }
}


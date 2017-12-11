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
public struct IndexBinding {
    public typealias Subfix = String

    let columnIndexes: [ColumnIndex]
    let isUnique: Bool

    public init(isUnique: Bool = false, indexesBy columnIndexConvertibleList: [ColumnIndexConvertible]) {
        self.columnIndexes = columnIndexConvertibleList.asIndexes()
        self.isUnique = isUnique
    }

    public init(isUnique: Bool = false,
                indexesBy columnIndexConvertibleList: ColumnIndexConvertible...) {
        self.init(isUnique: isUnique, indexesBy: columnIndexConvertibleList)
    }

    func generateCreateIndexStatement(onTable tableName: String,
                                      withIndexSubfix indexSubfix: String) -> StatementCreateIndex {
        return StatementCreateIndex()
            .create(index: tableName+indexSubfix,
                    isUnique: isUnique,
                    ifNotExists: true)
            .on(table: tableName,
                indexesBy: columnIndexes)
    }
}

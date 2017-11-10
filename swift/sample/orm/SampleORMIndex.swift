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

import WCDB

class SampleORMIndex: WCDB.TableCoding {
    var indexProperty: Int? = nil
    var uniqueIndexProperty: Int? = nil
    var multiIndexPart1: String? = nil
    var multiIndexPart2: Float? = nil
    
    required init() {}

    //WCDB
    static let objectRelationalMapping: TableBinding = TableBinding(SampleORMIndex.self)
}

//WCDB
extension SampleORMIndex {
    static func columnBindings() -> [AnyColumnBinding] {
        return [
            ColumnBinding(\SampleORMIndex.indexProperty),
            ColumnBinding(\SampleORMIndex.uniqueIndexProperty),
            ColumnBinding(\SampleORMIndex.multiIndexPart1),
            ColumnBinding(\SampleORMIndex.multiIndexPart2)]
    }
    
    static func indexBindings() -> [IndexBinding]? {
        return [
            IndexBinding(withSubfix: "_indexSubfix",
                         indexesBy: \SampleORMIndex.indexProperty),
            IndexBinding(withSubfix: "_uniqueIndexSubfix",
                         isUnique: true,
                         indexesBy: \SampleORMIndex.uniqueIndexProperty),
            IndexBinding(withSubfix: "_multiIndexSubfix",
                         indexesBy: \SampleORMIndex.multiIndexPart1, \SampleORMIndex.multiIndexPart2)]
    }
}

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

import WCDBSwift

class SampleORMIndex: TableCodable {
    var indexProperty: Int? = nil
    var uniqueIndexProperty: Int? = nil
    var multiIndexPart1: String? = nil
    var multiIndexPart2: Float? = nil
    enum CodingKeys: String, CodingTableKey {
        typealias Root = SampleORMIndex    
        static let __objectRelationalMapping = TableBinding(CodingKeys.self)    
        case indexProperty
        case uniqueIndexProperty
        case multiIndexPart1
        case multiIndexPart2
        static var __indexBindings: [IndexBinding.Subfix:IndexBinding]? {
            return [
                "_indexSubfix":IndexBinding(indexesBy: CodingKeys.indexProperty),
                "_uniqueIndexSubfix":IndexBinding(indexesBy: CodingKeys.uniqueIndexProperty),
                "_multiIndexSubfix":IndexBinding(indexesBy: CodingKeys.multiIndexPart1, CodingKeys.multiIndexPart2),
            ]
        }
    }
    required init() {}
}


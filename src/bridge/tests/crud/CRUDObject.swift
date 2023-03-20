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
import WCDB

extension CRUDObject: Named, Comparable {
    static func == (lhs: CRUDObject, rhs: CRUDObject) -> Bool {
        return lhs.variable1 == rhs.variable1 && lhs.variable2 == rhs.variable2
    }
    public static func < (lhs: CRUDObject, rhs: CRUDObject) -> Bool {
        if lhs.variable1 == rhs.variable1 {
            if let lhsv2 = lhs.variable2, let rhsv2 = rhs.variable2 {
                return lhsv2 < rhsv2
            }
            return lhs.variable2 == nil
        }
        return lhs.variable1 < rhs.variable1
    }
    public static func nameForMultiSelect() -> String {
        return "MultiSelect_\(name)"
    }
}

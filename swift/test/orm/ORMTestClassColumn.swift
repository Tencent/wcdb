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

class ORMTestClassColumn: WCDB.CodableClassColumn {

    var value1: Int = 1
    var value2: Float? = nil

    required init() {}   

    typealias FundamentalType = Data
    
    required init?(withTypedValue optionalValue: Data?) { 
        guard let value = optionalValue else {
            return nil
        }
        guard let result = try? JSONDecoder().decode([String:Codable].self, from: value) else {
            return nil
        }
        value1 = result["value1"] as? Int ?? 1
        value2 = result["value2"] as? Float
    }        
    
    func archivedTypedValue() -> Data? {
        return try? JSONEncoder().encode(["value1":value1, "value2":value2 as Any])
    }
}

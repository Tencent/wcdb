//
// Created by 陈秋文 on 2022/12/6.
//

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

class DatabaseTestCase: BaseTestCase {
    var database: Database!
    override func setUp() {
        super.setUp()
        database = Database(at: self.recommendedPath)
        database.tag = recommendTag
        Database.globalTrace { error in
            assert(error.level != .Fatal)
            print("Time: \(Date()),\(error)")
        }
    }

    override func tearDown() {
        Database.globalTrace(ofError: nil)
    }
}

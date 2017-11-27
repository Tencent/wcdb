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

import XCTest
import WCDB

class ModuleArgumentTests: WCDBTestCase {

    func testModuleArgument() {
        WINQAssertEqual(ModuleArgument(left: "left", right: "right"), "left=right")
        
        WINQAssertEqual(ModuleArgument(with: ColumnDef(with: Column(named: "column1"), and: .Integer32)), "column1 INTEGER")
        
        WINQAssertEqual(ModuleArgument(with: TableConstraint(named: "constraint1")), "CONSTRAINT constraint1")
        
        WINQAssertEqual(ModuleArgument(with: .WCDB), "tokenize=WCDB")
        
        WINQAssertEqual(ModuleArgument(withTokenize: "WCDB"), "tokenize=WCDB")
    }
}

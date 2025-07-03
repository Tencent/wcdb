//
// Created by qiuwenchen on 2022/11/10.
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
import WCDB_Private

public final class VirtualTableConfig: TableConfiguration {
    let module: String
    var parameters: [String]

    public func config(with tableBinding: TableBindingBase) {
        WCDBBindingConfigVirtualModule(tableBinding.cppBinding, module.cString)
        for parameter in parameters {
            WCDBBindingConfigVirtualModuleArgument(tableBinding.cppBinding, parameter.cString)
        }
    }

    public init(withModule version: FTSVersion, and tokenizer: String..., withExternalContent table: String? = nil) {
        self.module = version.description
        self.parameters = ["tokenize = \(tokenizer.joined(separateBy: " "))"]
        if let table = table {
            self.parameters.append("content='\(table)'")
        }
    }

    public init(withModule module: String, and parameters: String...) {
        self.module = module
        self.parameters = parameters
    }
}

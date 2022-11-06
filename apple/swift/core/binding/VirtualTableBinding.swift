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

public enum FTSVersion: Describable {
    case FTS3
    case FTS4
    case FTS5
    internal var cValue: WCDBFTSVersion {
        switch self {
        case .FTS3:
            return WCDBFTSVersion3
        case .FTS4:
            return WCDBFTSVersion4
        case .FTS5:
            return WCDBFTSVersion5
        }
    }
    public var description: String {
        switch self {
        case .FTS3:
            return "fts3"
        case .FTS4:
            return "fts4"
        case .FTS5:
            return "fts5"
        }
    }
}

public struct VirtualTableBinding {
    let module: String
    let parameters: [String]

    public init(withModule version: FTSVersion, and tokenizer: String...) {
        self.module = version.description
        self.parameters = ["tokenize = \(tokenizer.joined(separateBy: " "))"]
    }

    public init(withModule module: String, and parameters: String...) {
        self.module = module
        self.parameters = parameters
    }
}

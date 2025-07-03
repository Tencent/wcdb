//
// Created by qiuwenchen on 2022/5/29.
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

public enum Order {
    case ascending
    case descending
    internal var cValue: WCDBSyntaxOrder {
        switch self {
        case .ascending:
            return WCDBSyntaxOrder_Asc
        case .descending:
            return WCDBSyntaxOrder_Desc
        }
    }
}

public enum ConflictAction {
    case Replace
    case Rollback
    case Abort
    case Fail
    case Ignore
    internal var cValue: WCDBSyntaxConflictAction {
        switch self {
        case .Replace:
            return WCDBSyntaxConflictAction_Replace
        case .Rollback:
            return WCDBSyntaxConflictAction_Rollback
        case .Abort:
            return WCDBSyntaxConflictAction_Abort
        case .Fail:
            return WCDBSyntaxConflictAction_Fail
        case .Ignore:
            return WCDBSyntaxConflictAction_Ignore
        }
    }
}

public enum ColumnType {
    case integer32
    case integer64
    case text
    case float
    case BLOB
    case null
    internal var cValue: WCDBSyntaxColumnType {
        switch self {
        case .integer32:
            fallthrough
        case .integer64:
            return WCDBSyntaxColumnType_Integer
        case .text:
            return WCDBSyntaxColumnType_Text
        case .float:
            return WCDBSyntaxColumnType_Float
        case .BLOB:
            return WCDBSyntaxColumnType_BLOB
        case .null:
            return WCDBSyntaxColumnType_Null
        }
    }
}

public enum TransactionType {
    case deferred
    case immediate
    case exclusive
    internal var cValue: WCDBSyntaxTransactionType {
        switch self {
        case .deferred:
            return WCDBSyntaxTransactionType_Deferred
        case .immediate:
            return WCDBSyntaxTransactionType_Immediate
        case .exclusive:
            return WCDBSyntaxTransactionType_Exclusive
        }
    }
}

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

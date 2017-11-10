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

public class Statement : Describable {
    public enum StatementType {
        case AlterTable
        case CreateIndex
        case CreateTable
        case Delete
        case DropIndex
        case DropTable
        case Insert
        case Pragma
        case Select
        case Transaction
        case Update
        case CreateVirtualTable
        case Attach
        case Detach
        case Explain
        case Savepoint
        case Release
        case Rollback
        case Vacuum
        case Reindex
    }
    
    public let statementType: StatementType
    
    public init(with statementType: StatementType) {
        self.statementType = statementType
        super.init("")
    }
}

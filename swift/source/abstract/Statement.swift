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

public enum StatementType {
    case alterTable
    case createIndex
    case createTable
    case delete
    case dropIndex
    case dropTable
    case insert
    case pragma
    case select
    case transaction
    case update
    case createVirtualTable
    case attach
    case detach
    case explain
    case savepoint
    case release
    case rollback
    case vacuum
    case reindex
}

public protocol Statement: Describable {
    var statementType: StatementType {get}
}

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

final class CommonStatement {
    public static let enableFullfsync = StatementPragma().pragma(.fullfsync, to: true)

    public static let getJournalMode = StatementPragma().pragma(.journalMode)
    public static let enableJournalModeWAL = StatementPragma().pragma(.journalMode, to: "WAL")

    public static let enableSynchronousNormal = StatementPragma().pragma(.synchronous, to: "NORMAL")
    public static let enableSynchronousFull = StatementPragma().pragma(.synchronous, to: "FULL")

    public static let getLockingMode = StatementPragma().pragma(.lockingMode)
    public static let enableLockingModeNormal = StatementPragma().pragma(.lockingMode, to: "NORMAL")

    public static let checkpoint = StatementPragma().pragma(.walCheckpoint)

    public static let fts3Tokenizer = StatementSelect()
        .select(Expression.function(named: "fts3_tokenizer",
                                    Array(repeating: Expression.bindParameter, count: 2))
    )

    public static let beginTransactionImmediate = StatementTransaction().begin(.immediate)
    public static let commitTransaction = StatementTransaction().commit()
    public static let rollbackTransaction = StatementTransaction().rollback()
}

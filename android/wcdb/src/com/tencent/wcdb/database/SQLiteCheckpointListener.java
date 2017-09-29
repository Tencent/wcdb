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

package com.tencent.wcdb.database;

/**
 * A listener for WAL checkpointing.
 */
public interface SQLiteCheckpointListener {

    /**
     * Called immediately when this listener is set to the database. Initialization codes
     * can be placed here, if any.
     *
     * @param db database object to be attached
     */
    void onAttach(SQLiteDatabase db);

    /**
     * Called immediately when a WAL transaction has been committed.
     *
     * <p>The implementation should checkpoint the database according to its strategy by executing
     * SQL {@code PRAGMA wal_checkpoint;} or something alike. Failing to do this will result in
     * unlimited growing WAL file and may degrade performance.</p>
     *
     * @param db     database object whose transaction is committed
     * @param dbName attached database name of the committed transaction, or "main" for the
     *               main database
     * @param pages  number of frames in the WAL file after commit
     */
    void onWALCommit(SQLiteDatabase db, String dbName, int pages);

    /**
     * Called immediately when this listener is detached from the database. Cleanup codes
     * can be placed here, if any.
     *
     * @param db database object to be detached from
     */
    void onDetach(SQLiteDatabase db);
}

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
 * Listener interface for database change notifications.
 */
public interface SQLiteChangeListener {

    /**
     * Called when transactions are committed and some of rows in the database
     * is inserted, updated or deleted.
     *
     * When a transaction modifies more than one table, this method will be
     * called multiple times with different arguments.
     *
     * It's possible that multiple threads call this method concurrently, so
     * the implementation must be thread-safe.
     *
     * @param db        database object whose tables were changed
     * @param dbName    attached database name of the changed table, "main" for the main database
     * @param table     name of the changed table
     * @param insertIds array of rowIDs of the rows inserted, or null if rowID trace is disabled
     * @param updateIds array of rowIDs of the rows updated, or null if rowID trace is disabled
     * @param deleteIds array of rowIDs of the rows deleted, or null if rowID trace is disabled
     */
    void onChange(SQLiteDatabase db, String dbName, String table,
            long[] insertIds, long[] updateIds, long[] deleteIds);
}

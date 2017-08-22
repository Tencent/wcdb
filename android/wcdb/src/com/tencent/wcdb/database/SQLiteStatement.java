/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.tencent.wcdb.database;

import android.database.SQLException;
import com.tencent.wcdb.support.CancellationSignal;
import com.tencent.wcdb.support.OperationCanceledException;

/**
 * Represents a statement that can be executed against a database.  The statement
 * cannot return multiple rows or columns, but single value (1 x 1) result sets
 * are supported.
 * <p>
 * This class is not thread-safe.
 * </p>
 */
public final class SQLiteStatement extends SQLiteProgram {
    SQLiteStatement(SQLiteDatabase db, String sql, Object[] bindArgs) {
        super(db, sql, bindArgs, null);
    }

    /**
     * Execute this SQL statement, if it is not a SELECT / INSERT / DELETE / UPDATE, for example
     * CREATE / DROP table, view, trigger, index etc.
     *
     * @throws SQLException If the SQL string is invalid for some reason
     */
    public void execute() {
        execute(null);
    }

    /**
     * Execute this SQL statement, if it is not a SELECT / INSERT / DELETE / UPDATE, for example
     * CREATE / DROP table, view, trigger, index etc.
     *
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @throws SQLException If the SQL string is invalid for some reason
     */
    public void execute(CancellationSignal cancellationSignal) {
        acquireReference();
        try {
            getSession().execute(getSql(), getBindArgs(), getConnectionFlags(), cancellationSignal);
        } catch (SQLiteDatabaseCorruptException ex) {
            onCorruption();
            throw ex;
        } finally {
            releaseReference();
        }
    }

    /**
     * Execute this SQL statement, if the the number of rows affected by execution of this SQL
     * statement is of any importance to the caller - for example, UPDATE / DELETE SQL statements.
     *
     * @return the number of rows affected by this SQL statement execution.
     * @throws SQLException If the SQL string is invalid for some reason
     */
    public int executeUpdateDelete() {
        return executeUpdateDelete(null);
    }

    /**
     * Execute this SQL statement, if the the number of rows affected by execution of this SQL
     * statement is of any importance to the caller - for example, UPDATE / DELETE SQL statements.
     *
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @return the number of rows affected by this SQL statement execution.
     * @throws SQLException If the SQL string is invalid for some reason
     */
    public int executeUpdateDelete(CancellationSignal cancellationSignal) {
        acquireReference();
        try {
            return getSession().executeForChangedRowCount(
                    getSql(), getBindArgs(), getConnectionFlags(), cancellationSignal);
        } catch (SQLiteDatabaseCorruptException ex) {
            onCorruption();
            throw ex;
        } finally {
            releaseReference();
        }
    }

    /**
     * Execute this SQL statement and return the ID of the row inserted due to this call.
     * The SQL statement should be an INSERT for this to be a useful call.
     *
     * @return the row ID of the last row inserted, if this insert is successful. -1 otherwise.
     *
     * @throws SQLException If the SQL string is invalid for some reason
     */
    public long executeInsert() {
        return executeInsert(null);
    }

    /**
     * Execute this SQL statement and return the ID of the row inserted due to this call.
     * The SQL statement should be an INSERT for this to be a useful call.
     *
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @return the row ID of the last row inserted, if this insert is successful. -1 otherwise.
     *
     * @throws SQLException If the SQL string is invalid for some reason
     */
    public long executeInsert(CancellationSignal cancellationSignal) {
        acquireReference();
        try {
            return getSession().executeForLastInsertedRowId(
                    getSql(), getBindArgs(), getConnectionFlags(), cancellationSignal);
        } catch (SQLiteDatabaseCorruptException ex) {
            onCorruption();
            throw ex;
        } finally {
            releaseReference();
        }
    }

    /**
     * Execute a statement that returns a 1 by 1 table with a numeric value.
     * For example, SELECT COUNT(*) FROM table;
     *
     * @return The result of the query.
     *
     * @throws SQLiteDoneException if the query returns zero rows
     */
    public long simpleQueryForLong() {
        return simpleQueryForLong(null);
    }

    /**
     * Execute a statement that returns a 1 by 1 table with a numeric value.
     * For example, SELECT COUNT(*) FROM table;
     *
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @return The result of the query.
     *
     * @throws SQLiteDoneException if the query returns zero rows
     */
    public long simpleQueryForLong(CancellationSignal cancellationSignal) {
        acquireReference();
        try {
            return getSession().executeForLong(
                    getSql(), getBindArgs(), getConnectionFlags(), cancellationSignal);
        } catch (SQLiteDatabaseCorruptException ex) {
            onCorruption();
            throw ex;
        } finally {
            releaseReference();
        }
    }

    /**
     * Execute a statement that returns a 1 by 1 table with a text value.
     * For example, SELECT COUNT(*) FROM table;
     *
     * @return The result of the query.
     *
     * @throws SQLiteDoneException if the query returns zero rows
     */
    public String simpleQueryForString() {
        return simpleQueryForString(null);
    }

    /**
     * Execute a statement that returns a 1 by 1 table with a text value.
     * For example, SELECT COUNT(*) FROM table;
     *
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @return The result of the query.
     *
     * @throws SQLiteDoneException if the query returns zero rows
     */
    public String simpleQueryForString(CancellationSignal cancellationSignal) {
        acquireReference();
        try {
            return getSession().executeForString(
                    getSql(), getBindArgs(), getConnectionFlags(), cancellationSignal);
        } catch (SQLiteDatabaseCorruptException ex) {
            onCorruption();
            throw ex;
        } finally {
            releaseReference();
        }
    }

    @Override
    public String toString() {
        return "SQLiteProgram: " + getSql();
    }
}

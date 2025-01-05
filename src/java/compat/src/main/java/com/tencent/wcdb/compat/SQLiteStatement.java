package com.tencent.wcdb.compat;

import android.database.sqlite.SQLiteClosable;
import android.database.sqlite.SQLiteDoneException;
import android.os.CancellationSignal;
import android.text.TextUtils;

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.core.PreparedStatement;

import java.util.ArrayList;
import java.util.Arrays;

public final class SQLiteStatement extends SQLiteClosable {
    private final Database mDB;
    private final String mSql;
    private final ArrayList<Object> mBindArgs = new ArrayList<>();

    SQLiteStatement(Database db, String sql, Object[] bindArgs) {
        mDB = db;
        mSql = sql;
        if (bindArgs != null) {
            mBindArgs.addAll(Arrays.asList(bindArgs));
        }
    }

    private void execute(Handle handle, CancellationSignal cancellationSignal) {
        DatabaseUtils.bindCancellationSignal(handle, cancellationSignal);

        PreparedStatement stmt = handle.preparedWithMainStatement(mSql);
        int i = 1;
        for (Object arg : mBindArgs) {
            stmt.bindValue(new Value(arg), i++);
        }
        do {
            stmt.step();
        } while (!stmt.isDone());
        stmt.finalizeStatement();
    }

    private Value executeForValue(Handle handle, CancellationSignal cancellationSignal) {
        DatabaseUtils.bindCancellationSignal(handle, cancellationSignal);

        PreparedStatement stmt = handle.preparedWithMainStatement(mSql);
        int i = 1;
        for (Object arg : mBindArgs) {
            stmt.bindValue(new Value(arg), i++);
        }
        stmt.step();
        if (stmt.isDone()) {
            throw new SQLiteDoneException();
        }
        Value result = stmt.getValue(0);
        stmt.finalizeStatement();
        return result;
    }

    /**
     * @see #execute(CancellationSignal)
     * Executes a readonly database-operation without support for cancellation.
     */
    public int execute() {
        return execute(null);
    }

    /**
     * Executes a readonly database-operation with support for cancellation.
     *
     * @param cancellationSignal The CancellationSignal to cancel the operation if requested.
     *                           This allows the operation to be stopped in progress, useful for
     *                           long-running queries or operations that the user may wish to cancel.
     * @return The number of changes made by the operation if it is not read-only; otherwise, returns 0.
     * This provides insight into how many rows were affected in the case of a modification.
     * @throws android.database.SQLException If the SQL string is invalid for
     *                                       some reason
     */
    public int execute(CancellationSignal cancellationSignal) {
        try (Handle handle = mDB.getHandle(!TextUtils.isEmpty(mSql) && !mSql.toLowerCase().startsWith("select"))) {
            execute(handle, cancellationSignal);
            return handle.getChanges();
        }
    }

    /**
     * Execute this SQL statement, if the number of rows affected by execution of this SQL
     * statement is of any importance to the caller - for example, UPDATE / DELETE SQL statements.
     *
     * @return the number of rows affected by this SQL statement execution.
     * @throws android.database.SQLException If the SQL string is invalid for
     *                                       some reason
     */
    public int executeUpdateDelete() {
        return executeUpdateDelete(null);
    }

    public int executeUpdateDelete(CancellationSignal cancellationSignal) {
        try (Handle handle = mDB.getHandle(true)) {
            execute(handle, cancellationSignal);
            return handle.getChanges();
        }
    }

    /**
     * Execute this SQL statement and return the ID of the row inserted due to this call.
     * The SQL statement should be an INSERT for this to be a useful call.
     *
     * @return the row ID of the last row inserted, if this insert is successful. -1 otherwise.
     * @throws android.database.SQLException If the SQL string is invalid for
     *                                       some reason
     */
    public long executeInsert() {
        return executeInsert(null);
    }

    public long executeInsert(CancellationSignal cancellationSignal) {
        try (Handle handle = mDB.getHandle(true)) {
            execute(handle, cancellationSignal);
            return handle.getLastInsertedRowId();
        }
    }

    /**
     * Execute a statement that returns a 1 by 1 table with a numeric value.
     * For example, SELECT COUNT(*) FROM table;
     *
     * @return The result of the query.
     * @throws android.database.sqlite.SQLiteDoneException if the query returns zero rows
     */
    public long simpleQueryForLong() {
        return simpleQueryForLong(null);
    }

    public long simpleQueryForLong(CancellationSignal cancellationSignal) {
        // since just query, we can use read-only mode
        try (Handle handle = mDB.getHandle(false)) {
            return executeForValue(handle, cancellationSignal).getLong();
        }
    }

    /**
     * Execute a statement that returns a 1 by 1 table with a text value.
     * For example, SELECT COUNT(*) FROM table;
     *
     * @return The result of the query.
     * @throws android.database.sqlite.SQLiteDoneException if the query returns zero rows
     */
    public String simpleQueryForString() {
        return simpleQueryForString(null);
    }

    public String simpleQueryForString(CancellationSignal cancellationSignal) {
        try (Handle handle = mDB.getHandle(false)) {
            return executeForValue(handle, cancellationSignal).getText();
        }
    }

    /**
     * Bind a NULL value to this statement. The value remains bound until
     * {@link #clearBindings} is called.
     *
     * @param index The 1-based index to the parameter to bind null to
     */
    public void bindNull(int index) {
        bind(index, null);
    }

    /**
     * Bind a long value to this statement. The value remains bound until
     * {@link #clearBindings} is called.
     * addToBindArgs
     *
     * @param index The 1-based index to the parameter to bind
     * @param value The value to bind
     */
    public void bindLong(int index, long value) {
        bind(index, value);
    }

    /**
     * Bind a double value to this statement. The value remains bound until
     * {@link #clearBindings} is called.
     *
     * @param index The 1-based index to the parameter to bind
     * @param value The value to bind
     */
    public void bindDouble(int index, double value) {
        bind(index, value);
    }

    /**
     * Bind a String value to this statement. The value remains bound until
     * {@link #clearBindings} is called.
     *
     * @param index The 1-based index to the parameter to bind
     * @param value The value to bind, must not be null
     */
    public void bindString(int index, String value) {
        if (value == null) {
            throw new IllegalArgumentException("the bind value at index " + index + " is null");
        }
        bind(index, value);
    }

    /**
     * Bind a byte array value to this statement. The value remains bound until
     * {@link #clearBindings} is called.
     *
     * @param index The 1-based index to the parameter to bind
     * @param value The value to bind, must not be null
     */
    public void bindBlob(int index, byte[] value) {
        if (value == null) {
            throw new IllegalArgumentException("the bind value at index " + index + " is null");
        }
        bind(index, value);
    }

    public void bindAllArgs(Object[] bindArgs) {
        if (bindArgs == null) return;

        int length = bindArgs.length;
        if (mBindArgs.size() < length) {
            mBindArgs.ensureCapacity(length);
            do {
                mBindArgs.add(null);
            } while (mBindArgs.size() < length);
        }
        for (int i = 0; i < length; ++i) {
            mBindArgs.set(i, bindArgs[i]);
        }
    }

    /**
     * Clears all existing bindings. Unset bindings are treated as NULL.
     */
    public void clearBindings() {
        mBindArgs.clear();
    }

    @Override
    protected void onAllReferencesReleased() {
        clearBindings();
    }

    private void bind(int index, Object value) {
        if (index < 1) {
            throw new IllegalArgumentException("Cannot bind argument at index "
                    + index + " because the index is out of range.");
        }

        if (mBindArgs.size() < index) {
            mBindArgs.ensureCapacity(index);
            do {
                mBindArgs.add(null);
            } while (mBindArgs.size() < index);
        }
        mBindArgs.set(index - 1, value);
    }

    @Override
    public String toString() {
        return "SQLiteProgram: " + mSql;
    }
}

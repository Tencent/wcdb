package com.tencent.wcdb.compat;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteQueryBuilder;
import android.database.sqlite.SQLiteTransactionListener;
import android.os.CancellationSignal;
import android.os.OperationCanceledException;
import android.text.TextUtils;

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.base.WCDBInterruptException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.core.PreparedStatement;
import com.tencent.wcdb.winq.StatementInsert;

import org.jetbrains.annotations.NotNull;

import java.util.List;

/**
 * Exposes methods to manage a SQLite database.
 * <p/>
 * <p>
 * SQLiteDatabase has methods to create, delete, execute SQL commands, and
 * perform other common database management tasks.
 * </p><p>
 * See the Notepad sample application in the SDK for an example of creating
 * and managing a database.
 * </p><p>
 * Database names must be unique within an application, not across all applications.
 * </p>
 * <p/>
 * <h3>Localized Collation - ORDER BY</h3>
 * <p>
 * In addition to SQLite's default <code>BINARY</code> collator, Android supplies
 * two more, <code>LOCALIZED</code>, which changes with the system's current locale,
 * and <code>UNICODE</code>, which is the Unicode Collation Algorithm and not tailored
 * to the current locale.
 * </p>
 */
public final class SQLiteDatabase {
    private final Database mDB;

    SQLiteDatabase(Database db) {
        mDB = db;
    }

    /**
     * When a constraint violation occurs, an immediate ROLLBACK occurs,
     * thus ending the current transaction, and the command aborts with a
     * return code of SQLITE_CONSTRAINT. If no transaction is active
     * (other than the implied transaction that is created on every command)
     * then this algorithm works the same as ABORT.
     */
    public static final int CONFLICT_ROLLBACK = 1;

    /**
     * When a constraint violation occurs,no ROLLBACK is executed
     * so changes from prior commands within the same transaction
     * are preserved. This is the default behavior.
     */
    public static final int CONFLICT_ABORT = 2;

    /**
     * When a constraint violation occurs, the command aborts with a return
     * code SQLITE_CONSTRAINT. But any changes to the database that
     * the command made prior to encountering the constraint violation
     * are preserved and are not backed out.
     */
    public static final int CONFLICT_FAIL = 3;

    /**
     * When a constraint violation occurs, the one row that contains
     * the constraint violation is not inserted or changed.
     * But the command continues executing normally. Other rows before and
     * after the row that contained the constraint violation continue to be
     * inserted or updated normally. No error is returned.
     */
    public static final int CONFLICT_IGNORE = 4;

    /**
     * When a UNIQUE constraint violation occurs, the pre-existing rows that
     * are causing the constraint violation are removed prior to inserting
     * or updating the current row. Thus the insert or update always occurs.
     * The command continues executing normally. No error is returned.
     * If a NOT NULL constraint violation occurs, the NULL value is replaced
     * by the default value for that column. If the column has no default
     * value, then the ABORT algorithm is used. If a CHECK constraint
     * violation occurs then the IGNORE algorithm is used. When this conflict
     * resolution strategy deletes rows in order to satisfy a constraint,
     * it does not invoke delete triggers on those rows.
     * This behavior might change in a future release.
     */
    public static final int CONFLICT_REPLACE = 5;

    /**
     * Use the following when no conflict action is specified.
     */
    public static final int CONFLICT_NONE = 0;

    private static final String[] CONFLICT_VALUES = new String[]
            {"", " OR ROLLBACK ", " OR ABORT ", " OR FAIL ", " OR IGNORE ", " OR REPLACE "};


    public Database getInnerDB() {
        return mDB;
    }

    /**
     * Attempts to release memory that SQLite holds but does not require to
     * operate properly. Typically this memory will come from the page cache.
     *
     * @return the number of bytes actually released
     */
    public static int releaseMemory() {
        Database.purgeAll();
        // TODO: implement bytes actually released
        return 0;
    }

    private static final class Transaction {
        Transaction parent;
        SQLiteTransactionListener listener;
        boolean markedSuccessful;
        boolean childFailed;
    }

    private final ThreadLocal<Transaction> mTransactionStack = new ThreadLocal<>();

    private static void throwIfNoTransaction(Transaction stack) {
        if (stack == null) {
            throw new IllegalStateException("Cannot perform this operation because "
                    + "there is no current transaction.");
        }
    }

    private static void throwIfTransactionMarkedSuccessful(Transaction stack) {
        if (stack != null && stack.markedSuccessful) {
            throw new IllegalStateException("Cannot perform this operation because "
                    + "the transaction has already been marked successful.  The only "
                    + "thing you can do now is call endTransaction().");
        }
    }

    public boolean isOpen() {
        return mDB.isOpened();
    }

    /**
     * Close all handle of current database
     */
    public void close() {
        mDB.close();
    }

    /**
     * Begins a transaction in EXCLUSIVE mode.
     * <p>
     * Transactions can be nested.
     * When the outer transaction is ended all of
     * the work done in that transaction and all of the nested transactions will be committed or
     * rolled back. The changes will be rolled back if any transaction is ended without being
     * marked as clean (by calling setTransactionSuccessful). Otherwise they will be committed.
     * </p>
     * <p>Here is the standard idiom for transactions:
     * <p/>
     * <pre>
     *   db.beginTransaction();
     *   try {
     *     ...
     *     db.setTransactionSuccessful();
     *   } finally {
     *     db.endTransaction();
     *   }
     * </pre>
     */
    public void beginTransaction() {
        beginTransactionWithListener(null);
    }

    /**
     * Begins a transaction in IMMEDIATE mode. Transactions can be nested. When
     * the outer transaction is ended all of the work done in that transaction
     * and all of the nested transactions will be committed or rolled back. The
     * changes will be rolled back if any transaction is ended without being
     * marked as clean (by calling setTransactionSuccessful). Otherwise they
     * will be committed.
     * <p/>
     * Here is the standard idiom for transactions:
     * <p/>
     * <pre>
     *   db.beginTransactionNonExclusive();
     *   try {
     *     ...
     *     db.setTransactionSuccessful();
     *   } finally {
     *     db.endTransaction();
     *   }
     * </pre>
     */
    public void beginTransactionNonExclusive() {
        beginTransactionWithListener(null);
    }

    /**
     * Begins a transaction in EXCLUSIVE mode.
     * <p>
     * Transactions can be nested.
     * When the outer transaction is ended all of
     * the work done in that transaction and all of the nested transactions will be committed or
     * rolled back. The changes will be rolled back if any transaction is ended without being
     * marked as clean (by calling setTransactionSuccessful). Otherwise they will be committed.
     * </p>
     * <p>Here is the standard idiom for transactions:
     * <p/>
     * <pre>
     *   db.beginTransactionWithListener(listener);
     *   try {
     *     ...
     *     db.setTransactionSuccessful();
     *   } finally {
     *     db.endTransaction();
     *   }
     * </pre>
     *
     * @param transactionListener listener that should be notified when the transaction begins,
     *                            commits, or is rolled back, either explicitly or by a call to
     *                            {@link #yieldIfContendedSafely}.
     */
    public void beginTransactionWithListener(SQLiteTransactionListener transactionListener) {
        Transaction stack = mTransactionStack.get();
        throwIfTransactionMarkedSuccessful(stack);
        if (stack == null) {
            // No savepoint support here for compatibility
            mDB.beginTransaction();
        }

        if (transactionListener != null) {
            try {
                transactionListener.onBegin();
            } catch (RuntimeException e) {
                if (stack == null) {
                    mDB.rollbackTransaction();
                }
                throw e;
            }
        }

        Transaction transaction = new Transaction();
        transaction.listener = transactionListener;
        transaction.parent = stack;
        mTransactionStack.set(transaction);
    }

    /**
     * Begins a transaction in IMMEDIATE mode. Transactions can be nested. When
     * the outer transaction is ended all of the work done in that transaction
     * and all of the nested transactions will be committed or rolled back. The
     * changes will be rolled back if any transaction is ended without being
     * marked as clean (by calling setTransactionSuccessful). Otherwise they
     * will be committed.
     * <p/>
     * Here is the standard idiom for transactions:
     * <p/>
     * <pre>
     *   db.beginTransactionWithListenerNonExclusive(listener);
     *   try {
     *     ...
     *     db.setTransactionSuccessful();
     *   } finally {
     *     db.endTransaction();
     *   }
     * </pre>
     *
     * @param transactionListener listener that should be notified when the
     *                            transaction begins, commits, or is rolled back, either
     *                            explicitly or by a call to {@link #yieldIfContendedSafely}.
     */
    public void beginTransactionWithListenerNonExclusive(SQLiteTransactionListener transactionListener) {
        beginTransactionWithListener(transactionListener);
    }

    /**
     * End a transaction. See beginTransaction for notes about how to use this and when transactions
     * are committed and rolled back.
     */
    public void endTransaction() {
        Transaction stack = mTransactionStack.get();
        throwIfNoTransaction(stack);

        boolean successful = stack.markedSuccessful && !stack.childFailed;
        SQLiteTransactionListener listener = stack.listener;
        RuntimeException listenerException = null;
        if (listener != null) {
            try {
                if (successful) {
                    listener.onCommit();
                } else {
                    listener.onRollback();
                }
            } catch (RuntimeException e) {
                listenerException = e;
                successful = false;
            }
        }

        Transaction parent = stack.parent;
        mTransactionStack.set(parent);

        if (parent != null) {
            if (!successful) {
                parent.childFailed = true;
            }
        } else if (successful) {
            mDB.commitTransaction();
        } else {
            mDB.rollbackTransaction();
        }

        if (listenerException != null) {
            throw listenerException;
        }
    }

    /**
     * Marks the current transaction as successful. Do not do any more database work between
     * calling this and calling endTransaction. Do as little non-database work as possible in that
     * situation too. If any errors are encountered between this and endTransaction the transaction
     * will still be committed.
     *
     * @throws IllegalStateException if the current thread is not in a transaction or the
     *                               transaction is already marked as successful.
     */
    public void setTransactionSuccessful() {
        Transaction stack = mTransactionStack.get();
        throwIfNoTransaction(stack);
        throwIfTransactionMarkedSuccessful(stack);
        stack.markedSuccessful = true;
    }

    /**
     * Returns true if the current thread has a transaction pending.
     *
     * @return True if the current thread is in a transaction.
     */
    public boolean inTransaction() {
        return mDB.isInTransaction();
    }

    // TODO: implement yield methods

    /**
     * Gets the database version.
     *
     * @return the database version
     */
    public int getVersion() {
        return mDB.getValueFromSQL("PRAGMA user_version").getInt();
    }

    /**
     * Sets the database version.
     *
     * @param version the new database version
     */
    public void setVersion(int version) {
        mDB.execute("PRAGMA user_version = " + version);
    }

    /**
     * Returns the maximum size the database may grow to.
     *
     * @return the new maximum database size
     */
    public long getMaximumSize() {
        long pageCount = mDB.getValueFromSQL("PRAGMA max_page_count").getLong();
        return pageCount * getPageSize();
    }

    /**
     * Sets the maximum size the database will grow to. The maximum size cannot
     * be set below the current size.
     *
     * @param numBytes the maximum database size, in bytes
     * @return the new maximum database size
     * @deprecated use new {@code setConfig} methods to change configs to databases
     */
    public long setMaximumSize(long numBytes) {
        long pageSize = getPageSize();
        long numPages = numBytes / pageSize;
        // If numBytes isn't a multiple of pageSize, bump up a page
        if ((numBytes % pageSize) != 0) {
            numPages++;
        }
        long newPageCount = mDB.getValueFromSQL("PRAGMA max_page_count = " + numPages).getLong();
        return newPageCount * pageSize;
    }

    /**
     * Returns the current database page size, in bytes.
     *
     * @return the database page size, in bytes
     */
    public long getPageSize() {
        return mDB.getValueFromSQL("PRAGMA page_size").getLong();
    }

    /**
     * Sets the database page size. The page size must be a power of two. This
     * method does not work if any data has been written to the database file,
     * and must be called right after the database has been created.
     *
     * @param numBytes the database page size, in bytes
     * @deprecated use new {@code setConfig} methods to change configs to databases
     */
    public void setPageSize(long numBytes) {
        mDB.execute("PRAGMA page_size = " + numBytes);
    }

    /**
     * Compiles an SQL statement into a reusable pre-compiled statement object.
     * The parameters are identical to {@link #execSQL(String)}. You may put ?s in the
     * statement and fill in those values with {@link SQLiteStatement#bindString}
     * and {@link SQLiteStatement#bindLong} each time you want to run the
     * statement. Statements may not return result sets larger than 1x1.
     * <p>
     * No two threads should be using the same {@link SQLiteStatement} at the same time.
     *
     * @param sql The raw SQL statement, may contain ? for unknown values to be
     *            bound later.
     * @return A pre-compiled {@link SQLiteStatement} object. Note that
     * {@link SQLiteStatement}s are not synchronized, see the documentation for more details.
     */
    public SQLiteStatement compileStatement(String sql) {
        return new SQLiteStatement(mDB, sql, null);
    }

    /**
     * Query the given URL, returning a {@link Cursor} over the result set.
     *
     * @param distinct      true if you want each row to be unique, false otherwise.
     * @param table         The table name to compile the query against.
     * @param columns       A list of which columns to return. Passing null will
     *                      return all columns, which is discouraged to prevent reading
     *                      data from storage that isn't going to be used.
     * @param selection     A filter declaring which rows to return, formatted as an
     *                      SQL WHERE clause (excluding the WHERE itself). Passing null
     *                      will return all rows for the given table.
     * @param selectionArgs You may include ?s in selection, which will be
     *                      replaced by the values from selectionArgs, in order that they
     *                      appear in the selection. The values will be bound as Strings.
     * @param groupBy       A filter declaring how to group rows, formatted as an SQL
     *                      GROUP BY clause (excluding the GROUP BY itself). Passing null
     *                      will cause the rows to not be grouped.
     * @param having        A filter declare which row groups to include in the cursor,
     *                      if row grouping is being used, formatted as an SQL HAVING
     *                      clause (excluding the HAVING itself). Passing null will cause
     *                      all row groups to be included, and is required when row
     *                      grouping is not being used.
     * @param orderBy       How to order the rows, formatted as an SQL ORDER BY clause
     *                      (excluding the ORDER BY itself). Passing null will use the
     *                      default sort order, which may be unordered.
     * @param limit         Limits the number of rows returned by the query,
     *                      formatted as LIMIT clause. Passing null denotes no LIMIT clause.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     * @see Cursor
     */
    public Cursor query(boolean distinct, String table, String[] columns,
                        String selection, String[] selectionArgs, String groupBy,
                        String having, String orderBy, String limit) {
        return query(distinct, table, columns, selection, selectionArgs, groupBy, having, orderBy,
                limit, null);
    }

    /**
     * Query the given URL, returning a {@link Cursor} over the result set.
     *
     * @param distinct           true if you want each row to be unique, false otherwise.
     * @param table              The table name to compile the query against.
     * @param columns            A list of which columns to return. Passing null will
     *                           return all columns, which is discouraged to prevent reading
     *                           data from storage that isn't going to be used.
     * @param selection          A filter declaring which rows to return, formatted as an
     *                           SQL WHERE clause (excluding the WHERE itself). Passing null
     *                           will return all rows for the given table.
     * @param selectionArgs      You may include ?s in selection, which will be
     *                           replaced by the values from selectionArgs, in order that they
     *                           appear in the selection. The values will be bound as Strings.
     * @param groupBy            A filter declaring how to group rows, formatted as an SQL
     *                           GROUP BY clause (excluding the GROUP BY itself). Passing null
     *                           will cause the rows to not be grouped.
     * @param having             A filter declare which row groups to include in the cursor,
     *                           if row grouping is being used, formatted as an SQL HAVING
     *                           clause (excluding the HAVING itself). Passing null will cause
     *                           all row groups to be included, and is required when row
     *                           grouping is not being used.
     * @param orderBy            How to order the rows, formatted as an SQL ORDER BY clause
     *                           (excluding the ORDER BY itself). Passing null will use the
     *                           default sort order, which may be unordered.
     * @param limit              Limits the number of rows returned by the query,
     *                           formatted as LIMIT clause. Passing null denotes no LIMIT clause.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     * @see Cursor
     */
    public Cursor query(boolean distinct, String table, String[] columns,
                        String selection, String[] selectionArgs, String groupBy,
                        String having, String orderBy, String limit, CancellationSignal cancellationSignal) {
        String sql = SQLiteQueryBuilder.buildQueryString(
                distinct, table, columns, selection, groupBy, having, orderBy, limit);
        return rawQuery(sql, selectionArgs, cancellationSignal);
    }

    /**
     * Query the given table, returning a {@link Cursor} over the result set.
     *
     * @param table         The table name to compile the query against.
     * @param columns       A list of which columns to return. Passing null will
     *                      return all columns, which is discouraged to prevent reading
     *                      data from storage that isn't going to be used.
     * @param selection     A filter declaring which rows to return, formatted as an
     *                      SQL WHERE clause (excluding the WHERE itself). Passing null
     *                      will return all rows for the given table.
     * @param selectionArgs You may include ?s in selection, which will be
     *                      replaced by the values from selectionArgs, in order that they
     *                      appear in the selection. The values will be bound as Strings.
     * @param groupBy       A filter declaring how to group rows, formatted as an SQL
     *                      GROUP BY clause (excluding the GROUP BY itself). Passing null
     *                      will cause the rows to not be grouped.
     * @param having        A filter declare which row groups to include in the cursor,
     *                      if row grouping is being used, formatted as an SQL HAVING
     *                      clause (excluding the HAVING itself). Passing null will cause
     *                      all row groups to be included, and is required when row
     *                      grouping is not being used.
     * @param orderBy       How to order the rows, formatted as an SQL ORDER BY clause
     *                      (excluding the ORDER BY itself). Passing null will use the
     *                      default sort order, which may be unordered.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     * @see Cursor
     */
    public Cursor query(String table, String[] columns, String selection,
                        String[] selectionArgs, String groupBy, String having,
                        String orderBy) {

        return query(false, table, columns, selection, selectionArgs, groupBy,
                having, orderBy, null /* limit */);
    }

    /**
     * Query the given table, returning a {@link Cursor} over the result set.
     *
     * @param table         The table name to compile the query against.
     * @param columns       A list of which columns to return. Passing null will
     *                      return all columns, which is discouraged to prevent reading
     *                      data from storage that isn't going to be used.
     * @param selection     A filter declaring which rows to return, formatted as an
     *                      SQL WHERE clause (excluding the WHERE itself). Passing null
     *                      will return all rows for the given table.
     * @param selectionArgs You may include ?s in selection, which will be
     *                      replaced by the values from selectionArgs, in order that they
     *                      appear in the selection. The values will be bound as Strings.
     * @param groupBy       A filter declaring how to group rows, formatted as an SQL
     *                      GROUP BY clause (excluding the GROUP BY itself). Passing null
     *                      will cause the rows to not be grouped.
     * @param having        A filter declare which row groups to include in the cursor,
     *                      if row grouping is being used, formatted as an SQL HAVING
     *                      clause (excluding the HAVING itself). Passing null will cause
     *                      all row groups to be included, and is required when row
     *                      grouping is not being used.
     * @param orderBy       How to order the rows, formatted as an SQL ORDER BY clause
     *                      (excluding the ORDER BY itself). Passing null will use the
     *                      default sort order, which may be unordered.
     * @param limit         Limits the number of rows returned by the query,
     *                      formatted as LIMIT clause. Passing null denotes no LIMIT clause.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     * @see Cursor
     */
    public Cursor query(String table, String[] columns, String selection,
                        String[] selectionArgs, String groupBy, String having,
                        String orderBy, String limit) {

        return query(false, table, columns, selection, selectionArgs, groupBy,
                having, orderBy, limit);
    }

    /**
     * Runs the provided SQL and returns a {@link Cursor} over the result set.
     *
     * @param sql           the SQL query. The SQL string must not be ; terminated
     * @param selectionArgs You may include ?s in where clause in the query,
     *                      which will be replaced by the values from selectionArgs. The
     *                      values will be bound as Strings.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     */
    public Cursor rawQuery(String sql, Object[] selectionArgs) {
        return rawQuery(sql, selectionArgs, null);
    }

    /**
     * Runs the provided SQL and returns a {@link Cursor} over the result set.
     *
     * @param sql                the SQL query. The SQL string must not be ; terminated
     * @param selectionArgs      You may include ?s in where clause in the query,
     *                           which will be replaced by the values from selectionArgs. The
     *                           values will be bound as Strings.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     */
    public Cursor rawQuery(String sql, Object[] selectionArgs,
                           CancellationSignal cancellationSignal) {
        try (Handle handle = mDB.getHandle(false)) {
            DatabaseUtils.bindCancellationSignal(handle, cancellationSignal);

            PreparedStatement stmt = handle.preparedWithMainStatement(sql);
            if (selectionArgs != null) {
                int nArgs = selectionArgs.length;
                for (int i = 0; i < nArgs; ++i) {
                    stmt.bindValue(new Value(selectionArgs[i]), i + 1);
                }
            }

            String[] columnNames = new String[stmt.getColumnCount()];
            for (int i = 0; i < columnNames.length; ++i) {
                columnNames[i] = stmt.getColumnName(i);
            }

            List<Value[]> result = stmt.getMultiRows();
            stmt.finalizeStatement();
            return new ValueCursor(result, columnNames);
        } catch (WCDBInterruptException e) {
            throw (OperationCanceledException) (new OperationCanceledException().initCause(e));
        }
    }

    /**
     * Convenience method for inserting a row into the database.
     *
     * @param table          the table to insert the row into
     * @param nullColumnHack optional; may be <code>null</code>.
     *                       SQL doesn't allow inserting a completely empty row without
     *                       naming at least one column name.  If your provided <code>values</code> is
     *                       empty, no column names are known and an empty row can't be inserted.
     *                       If not set to null, the <code>nullColumnHack</code> parameter
     *                       provides the name of nullable column name to explicitly insert a NULL into
     *                       in the case where your <code>values</code> is empty.
     * @param values         this map contains the initial column values for the
     *                       row. The keys should be the column names and the values the
     *                       column values
     * @return the row ID of the newly inserted row, or -1 if an error occurred
     */
    public long insert(String table, String nullColumnHack, ContentValues values) {
        try {
            return insertWithOnConflict(table, nullColumnHack, values, CONFLICT_NONE);
        } catch (WCDBException e) {
            // TODO: print logs
            //Log.e(TAG, "Error inserting " + values, e);
            return -1;
        }
    }

    /**
     * Convenience method for inserting a row into the database.
     *
     * @param table          the table to insert the row into
     * @param nullColumnHack optional; may be <code>null</code>.
     *                       SQL doesn't allow inserting a completely empty row without
     *                       naming at least one column name.  If your provided <code>values</code> is
     *                       empty, no column names are known and an empty row can't be inserted.
     *                       If not set to null, the <code>nullColumnHack</code> parameter
     *                       provides the name of nullable column name to explicitly insert a NULL into
     *                       in the case where your <code>values</code> is empty.
     * @param values         this map contains the initial column values for the
     *                       row. The keys should be the column names and the values the
     *                       column values
     * @return the row ID of the newly inserted row, or -1 if an error occurred
     * @throws SQLException
     */
    public long insertOrThrow(String table, String nullColumnHack, ContentValues values)
            throws SQLException {
        return insertWithOnConflict(table, nullColumnHack, values, CONFLICT_NONE);
    }

    /**
     * Convenience method for replacing a row in the database.
     * Inserts a new row if a row does not already exist.
     *
     * @param table          the table in which to replace the row
     * @param nullColumnHack optional; may be <code>null</code>.
     *                       SQL doesn't allow inserting a completely empty row without
     *                       naming at least one column name.  If your provided <code>initialValues</code> is
     *                       empty, no column names are known and an empty row can't be inserted.
     *                       If not set to null, the <code>nullColumnHack</code> parameter
     *                       provides the name of nullable column name to explicitly insert a NULL into
     *                       in the case where your <code>initialValues</code> is empty.
     * @param initialValues  this map contains the initial column values for
     *                       the row. The keys should be the column names and the values the column values.
     * @return the row ID of the newly inserted row, or -1 if an error occurred
     */
    public long replace(String table, String nullColumnHack, ContentValues initialValues) {
        try {
            return insertWithOnConflict(table, nullColumnHack, initialValues,
                    CONFLICT_REPLACE);
        } catch (SQLException e) {
            // TODO: print logs
            //Log.e(TAG, "Error inserting " + initialValues, e);
            return -1;
        }
    }

    /**
     * Convenience method for replacing a row in the database.
     * Inserts a new row if a row does not already exist.
     *
     * @param table          the table in which to replace the row
     * @param nullColumnHack optional; may be <code>null</code>.
     *                       SQL doesn't allow inserting a completely empty row without
     *                       naming at least one column name.  If your provided <code>initialValues</code> is
     *                       empty, no column names are known and an empty row can't be inserted.
     *                       If not set to null, the <code>nullColumnHack</code> parameter
     *                       provides the name of nullable column name to explicitly insert a NULL into
     *                       in the case where your <code>initialValues</code> is empty.
     * @param initialValues  this map contains the initial column values for
     *                       the row. The keys should be the column names and the values the column values.
     * @return the row ID of the newly inserted row, or -1 if an error occurred
     * @throws SQLException
     */
    public long replaceOrThrow(String table, String nullColumnHack,
                               ContentValues initialValues) throws SQLException {
        return insertWithOnConflict(table, nullColumnHack, initialValues,
                CONFLICT_REPLACE);
    }

    /**
     * General method for inserting a row into the database.
     *
     * @param table             the table to insert the row into
     * @param nullColumnHack    optional; may be <code>null</code>.
     *                          SQL doesn't allow inserting a completely empty row without
     *                          naming at least one column name.  If your provided <code>initialValues</code> is
     *                          empty, no column names are known and an empty row can't be inserted.
     *                          If not set to null, the <code>nullColumnHack</code> parameter
     *                          provides the name of nullable column name to explicitly insert a NULL into
     *                          in the case where your <code>initialValues</code> is empty.
     * @param initialValues     this map contains the initial column values for the
     *                          row. The keys should be the column names and the values the
     *                          column values
     * @param conflictAlgorithm for insert conflict resolver
     * @return the row ID of the newly inserted row OR <code>-1</code> if either the
     * input parameter <code>conflictAlgorithm</code> = {@link #CONFLICT_IGNORE}
     * or an error occurred.
     */
    public long insertWithOnConflict(String table, String nullColumnHack,
                                     ContentValues initialValues, int conflictAlgorithm) {
        String[] columns;
        Object[] bindArgs;
        int size = (initialValues != null && initialValues.size() > 0)
                ? initialValues.size() : 0;
        if (size > 0) {
            bindArgs = new Object[size];
            columns = new String[size];
            int i = 0;
            for (String colName : initialValues.keySet()) {
                columns[i] = colName;
                bindArgs[i++] = initialValues.get(colName);
            }
        } else {
            columns = new String[]{nullColumnHack};
            bindArgs = new Object[]{null};
        }

        StatementInsert insert = new StatementInsert().insertInto(table);
        switch (conflictAlgorithm) {
            case CONFLICT_NONE:
                break;
            case CONFLICT_ROLLBACK:
                insert.orRollback();
                break;
            case CONFLICT_ABORT:
                insert.orAbort();
                break;
            case CONFLICT_FAIL:
                insert.orFail();
                break;
            case CONFLICT_IGNORE:
                insert.orIgnore();
                break;
            case CONFLICT_REPLACE:
                insert.orReplace();
                break;
        }
        insert.columns(columns).valuesWithBindParameters(bindArgs.length);
        try (Handle handle = mDB.getHandle(true)) {
            PreparedStatement stmt = handle.preparedWithMainStatement(insert);
            for (int i = 0; i < bindArgs.length; ++i) {
                stmt.bindValue(new Value(bindArgs[i]), i + 1);
            }
            do {
                stmt.step();
            } while (!stmt.isDone());
            stmt.finalizeStatement();
            return handle.getLastInsertedRowId();
        }
    }

    /**
     * Convenience method for deleting rows in the database.
     *
     * @param table       the table to delete from
     * @param whereClause the optional WHERE clause to apply when deleting.
     *                    Passing null will delete all rows.
     * @param whereArgs   You may include ?s in the where clause, which
     *                    will be replaced by the values from whereArgs. The values
     *                    will be bound as Strings.
     * @return the number of rows affected if a whereClause is passed in, 0
     * otherwise. To remove all rows and get a count pass "1" as the
     * whereClause.
     */
    public int delete(String table, String whereClause, String[] whereArgs) {
        String sql = "DELETE FROM " + table +
                (!TextUtils.isEmpty(whereClause) ? " WHERE " + whereClause : "");
        try (Handle handle = mDB.getHandle(true)) {
            PreparedStatement stmt = handle.preparedWithMainStatement(sql);
            if (whereArgs != null) {
                for (int i = 0; i < whereArgs.length; ++i) {
                    stmt.bindText(whereArgs[i], i + 1);
                }
            }
            do {
                stmt.step();
            } while (!stmt.isDone());
            stmt.finalizeStatement();
            return handle.getChanges();
        }
    }

    /**
     * Convenience method for updating rows in the database.
     *
     * @param table       the table to update in
     * @param values      a map from column names to new column values. null is a
     *                    valid value that will be translated to NULL.
     * @param whereClause the optional WHERE clause to apply when updating.
     *                    Passing null will update all rows.
     * @param whereArgs   You may include ?s in the where clause, which
     *                    will be replaced by the values from whereArgs. The values
     *                    will be bound as Strings.
     * @return the number of rows affected
     */
    public int update(String table, ContentValues values, String whereClause, String[] whereArgs) {
        return updateWithOnConflict(table, values, whereClause, whereArgs, CONFLICT_NONE);
    }

    /**
     * Convenience method for updating rows in the database.
     *
     * @param table             the table to update in
     * @param values            a map from column names to new column values. null is a
     *                          valid value that will be translated to NULL.
     * @param whereClause       the optional WHERE clause to apply when updating.
     *                          Passing null will update all rows.
     * @param whereArgs         You may include ?s in the where clause, which
     *                          will be replaced by the values from whereArgs. The values
     *                          will be bound as Strings.
     * @param conflictAlgorithm for update conflict resolver
     * @return the number of rows affected
     */
    public int updateWithOnConflict(String table, ContentValues values,
                                    String whereClause, String[] whereArgs, int conflictAlgorithm) {
        StringBuilder sql = new StringBuilder(120);
        sql.append("UPDATE ");
        sql.append(CONFLICT_VALUES[conflictAlgorithm]);
        sql.append(table);
        sql.append(" SET ");

        // move all bind args to one array
        int setValuesSize = values.size();
        int bindArgsSize = (whereArgs == null) ? setValuesSize : (setValuesSize + whereArgs.length);
        Object[] bindArgs = new Object[bindArgsSize];
        int i = 0;
        for (String colName : values.keySet()) {
            sql.append((i > 0) ? "," : "");
            sql.append(colName);
            bindArgs[i++] = values.get(colName);
            sql.append("=?");
        }
        if (whereArgs != null) {
            for (i = setValuesSize; i < bindArgsSize; i++) {
                bindArgs[i] = whereArgs[i - setValuesSize];
            }
        }
        if (!TextUtils.isEmpty(whereClause)) {
            sql.append(" WHERE ");
            sql.append(whereClause);
        }
        try (Handle handle = mDB.getHandle(true)) {
            PreparedStatement stmt = handle.preparedWithMainStatement(sql.toString());
            for (i = 0; i < bindArgsSize; ++i) {
                stmt.bindValue(new Value(bindArgs[i]), i + 1);
            }
            do {
                stmt.step();
            } while (!stmt.isDone());
            stmt.finalizeStatement();
            return handle.getChanges();
        }
    }

    /**
     * Execute a single SQL statement that is NOT a SELECT
     * or any other SQL statement that returns data.
     * <p>
     * It has no means to return any data (such as the number of affected rows).
     * Instead, you're encouraged to use {@link #insert(String, String, ContentValues)},
     * {@link #update(String, ContentValues, String, String[])}, et al, when possible.
     * </p>
     * <p>
     * When using {@link #enableWriteAheadLogging()}, journal_mode is
     * automatically managed by this class. So, do not set journal_mode
     * using "PRAGMA journal_mode'<value>" statement if your app is using
     * {@link #enableWriteAheadLogging()}
     * </p>
     * <p>
     * Note that {@code PRAGMA} values which apply on a per-connection basis
     * should <em>not</em> be configured using this method; you should instead
     * use {@link #execPerConnectionSQL} to ensure that they are uniformly
     * applied to all current and future connections.
     * </p>
     *
     * @param sql the SQL statement to be executed. Multiple statements separated by semicolons are
     *            not supported.
     * @throws SQLException if the SQL string is invalid
     */
    public void execSQL(String sql) {
        execSQL(sql, null);
    }

    /**
     * Execute a single SQL statement that is NOT a SELECT/INSERT/UPDATE/DELETE.
     * <p>
     * For INSERT statements, use any of the following instead.
     * <ul>
     *   <li>{@link #insert(String, String, ContentValues)}</li>
     *   <li>{@link #insertOrThrow(String, String, ContentValues)}</li>
     *   <li>{@link #insertWithOnConflict(String, String, ContentValues, int)}</li>
     * </ul>
     * <p>
     * For UPDATE statements, use any of the following instead.
     * <ul>
     *   <li>{@link #update(String, ContentValues, String, String[])}</li>
     *   <li>{@link #updateWithOnConflict(String, ContentValues, String, String[], int)}</li>
     * </ul>
     * <p>
     * For DELETE statements, use any of the following instead.
     * <ul>
     *   <li>{@link #delete(String, String, String[])}</li>
     * </ul>
     * <p>
     * For example, the following are good candidates for using this method:
     * <ul>
     *   <li>ALTER TABLE</li>
     *   <li>CREATE or DROP table / trigger / view / index / virtual table</li>
     *   <li>REINDEX</li>
     *   <li>RELEASE</li>
     *   <li>SAVEPOINT</li>
     *   <li>PRAGMA that returns no data</li>
     * </ul>
     * </p>
     * <p>
     * When using {@link #enableWriteAheadLogging()}, journal_mode is
     * automatically managed by this class. So, do not set journal_mode
     * using "PRAGMA journal_mode'<value>" statement if your app is using
     * {@link #enableWriteAheadLogging()}
     * </p>
     * <p>
     * Note that {@code PRAGMA} values which apply on a per-connection basis
     * should <em>not</em> be configured using this method; you should instead
     * use {@link #execPerConnectionSQL} to ensure that they are uniformly
     * applied to all current and future connections.
     * </p>
     *
     * @param sql      the SQL statement to be executed. Multiple statements separated by semicolons are
     *                 not supported.
     * @param bindArgs only byte[], String, Long and Double are supported in bindArgs.
     * @throws SQLException if the SQL string is invalid
     */
    public int execSQL(String sql, Object[] bindArgs) {
        return execSQL(sql, bindArgs, null);
    }

    public int execSQL(String sql, Object[] bindArgs, CancellationSignal cancellationSignal) {
        try (SQLiteStatement stmt = new SQLiteStatement(mDB, sql, bindArgs)) {
            return stmt.execute(cancellationSignal);
        }
    }

    /**
     * Gets the path to the database file.
     *
     * @return The path to the database file.
     */
    public String getPath() {
        return mDB.getPath();
    }

    @Override
    public String toString() {
        return "SQLiteDatabase: " + getPath();
    }

    public static SQLiteDatabase wrap(Database db) {
        return new SQLiteDatabase(db);
    }

    /**
     * Open the database according to the flags {@link #OPEN_READWRITE}
     * {@link #OPEN_READONLY} {@link #CREATE_IF_NECESSARY} and/or {@link #NO_LOCALIZED_COLLATORS}.
     * <p/>
     * <p>Sets the locale of the database to the  the system's current locale.
     * Call {@link #setLocale} if you would like something else.</p>
     *
     * @param path    to database file to open and/or create
     * @param factory an optional factory class that is called to instantiate a
     *                cursor when query is called, or null for default
     * @param flags   to control database access mode
     * @return the newly opened database
     * @throws SQLiteException if the database cannot be opened
     */
    public static SQLiteDatabase openDatabase(String path) {
        return openDatabase(path, null, null, null);
    }

    public static SQLiteDatabase openDatabase(String path, DatabaseErrorHandler errorHandler) {
        return openDatabase(path, null, null, errorHandler);
    }

    public static SQLiteDatabase openDatabase(String path, byte[] password, SQLiteCipherSpec cipher) {
        return openDatabase(path, password, cipher, null);
    }

    /**
     * Open the database according to the flags {@link #OPEN_READWRITE}
     * {@link #OPEN_READONLY} {@link #CREATE_IF_NECESSARY} and/or {@link #NO_LOCALIZED_COLLATORS}.
     * <p/>
     * <p>Sets the locale of the database to the  the system's current locale.
     * Call {@link #setLocale} if you would like something else.</p>
     * <p/>
     * <p>Accepts input param: a concrete instance of {@link DatabaseErrorHandler} to be
     * used to handle corruption when sqlite reports database corruption.</p>
     *
     * @param path         to database file to open and/or create
     * @param password     password to the encrypted database, or null for plain-text database
     * @param cipher       cipher description, or null for default settings
     * @param errorHandler the {@link DatabaseErrorHandler} obj to be used to handle corruption
     *                     when sqlite reports database corruption
     * @return the newly opened database
     * @throws SQLiteException if the database cannot be opened
     */
    public static SQLiteDatabase openDatabase(String path, byte[] password, final SQLiteCipherSpec cipher,
                                              final DatabaseErrorHandler errorHandler) {
        Database db = new Database(path);
        if (password != null) {
            if (cipher == null) {
                db.setCipherKey(password);
            } else {
                if (cipher.cipherVersion >= 0) {
                    db.setCipherKey(password, cipher.pageSize, kCipherVersionMapping[cipher.cipherVersion]);
                } else {
                    db.setCipherKey(password, cipher.pageSize);
                    db.setConfig("com.Tencent.WCDB.Config.CipherSpecCompat", new Database.Config() {
                        @Override
                        public void onInvocation(@NotNull Handle handle) throws WCDBException {
                            handle.execute("PRAGMA cipher_use_hmac = " + cipher.hmacEnabled);
                            if (cipher.kdfIteration != 0) {
                                handle.execute("PRAGMA kdf_iter = " + cipher.kdfIteration);
                            }
                            if (cipher.hmacAlgorithm != SQLiteCipherSpec.HMAC_DEFAULT) {
                                handle.execute("PRAGMA cipher_hmac_algorithm = " +
                                        SQLiteCipherSpec.HMAC_ALGO_MAPPING[cipher.hmacAlgorithm]);
                            }
                            if (cipher.kdfAlgorithm != SQLiteCipherSpec.HMAC_DEFAULT) {
                                handle.execute("PRAGMA cipher_kdf_algorithm = " +
                                        SQLiteCipherSpec.PBKDF2_ALGO_MAPPING[cipher.kdfAlgorithm]);
                            }
                        }
                    }, Database.ConfigPriority.highest);
                }
            }
        }

        final SQLiteDatabase dbObj = new SQLiteDatabase(db);
        Database.CorruptionNotification corruptionNotification = new Database.CorruptionNotification() {
            private final DatabaseErrorHandler mHandler = (errorHandler == null) ?
                    sDefaultErrorHandler : errorHandler;

            @Override
            public void onCorrupted(Database database) {
                mHandler.onCorruption(dbObj);
            }
        };
        db.setNotificationWhenCorrupted(corruptionNotification);

        if (!db.canOpen()) {
            corruptionNotification.onCorrupted(db);
            Handle handle = db.getHandle();     // May throw
            handle.invalidate();
        }
        return dbObj;
    }

    private static final Database.CipherVersion[] kCipherVersionMapping = Database.CipherVersion.values();

    private static final DatabaseErrorHandler sDefaultErrorHandler = new DatabaseErrorHandler() {
        @Override
        public void onCorruption(SQLiteDatabase dbObj) {
            Database db = dbObj.getInnerDB();
            db.removeFiles();
        }
    };
}

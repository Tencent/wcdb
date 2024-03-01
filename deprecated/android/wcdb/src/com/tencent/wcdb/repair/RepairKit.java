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

package com.tencent.wcdb.repair;

import android.database.Cursor;

import com.tencent.wcdb.AbstractCursor;
import com.tencent.wcdb.database.SQLiteCipherSpec;
import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.database.SQLiteException;
import com.tencent.wcdb.support.CancellationSignal;

import java.io.IOException;


/**
 * Database repair toolkit to parse a corrupted database file and
 * write its content to another (newly created) database.
 */
public class RepairKit implements CancellationSignal.OnCancelListener {
    private static final String TAG = "WCDB.RepairKit";

    /**
     * Result code that indicates successful operation.
     */
    public static final int RESULT_OK = 0;

    /**
     * Result code that indicates operation has been cancelled.
     */
    public static final int RESULT_CANCELED = 1;

    /**
     * Result code that indicates operation failure.
     */
    public static final int RESULT_FAILED = -1;

    public static final int RESULT_IGNORE = 2;

    /**
     * Flag indicates no {@code CREATE TABLE} or {@code CREATE INDEX} statement
     * should be executed on the destination database.
     */
    public static final int FLAG_NO_CREATE_TABLES = 0x0001;

    /**
     * Flag indicates all tables and indices should be written to the destination
     * database, regardless to how table filters are set.
     */
    public static final int FLAG_ALL_TABLES = 0x0002;

    /**
     * Flag indicates {@code CREATE TABLE} statements recovered from corrupted
     * databases should be check against those in backup files.
     */
    public static final int FLAG_CHECK_TABLE_COLUMNS = 0x0004;


    private static final int INTEGRITY_HEADER = 0x0001;
    private static final int INTEGRITY_DATA = 0x0002;
    private static final int INTEGRITY_KDF_SALT = 0x0004;

    private long mNativePtr;
    private int mIntegrityFlags;
    private MasterInfo mMasterInfo;
    private LeafInfo mLeafInfo;
    private Callback mCallback;
    private RepairCursor mCurrentCursor;


    /**
     * Open a corrupted database for repairing.
     *
     * <p>Database is successfully opened and ready to be parsed when the constructor returns
     * without throwing exceptions. The caller should catch {@link SQLiteException} in case of
     * failure due to file opening error, wrong password, or unrecoverable corruption.
     * </p>
     *
     * <p>Backup master info can be provided for better chance of recovery.
     * Backup master info can be saved before the database is corrupted by calling
     * {@link MasterInfo#save(SQLiteDatabase, String, byte[])}. To use backup master info
     * on recovery, call {@link MasterInfo#load(String, byte[], String[])} on saved info
     * and pass it as {@code master} argument.</p>
     *
     * <p>Table filters can be applied to recover only selected tables. Calling {@link
     * MasterInfo#make(String[])} or {@link MasterInfo#load(String, byte[], String[])}
     * to generate table filters.</p>
     *
     * @param path          path to the corrupted database to be repaired
     * @param key           password to the encrypted database, or null for plain-text database
     * @param cipherSpec    cipher description, or null for default settings
     * @param master        backup master info and/or table filters
     * @throws SQLiteException when corrupted database cannot be opened.
     * @throws IllegalArgumentException when path is null.
     */
    public RepairKit(String path, byte[] key, SQLiteCipherSpec cipherSpec, MasterInfo master) {
        this(path, key, cipherSpec, master, null);
    }

    public RepairKit(String path, byte[] key, SQLiteCipherSpec cipherSpec, MasterInfo master,
                     LeafInfo leaf) {
        if (path == null)
            throw new IllegalArgumentException();

        mNativePtr = nativeInit(path, key, cipherSpec, (master == null) ? null : master.mKDFSalt);
        if (mNativePtr == 0)
            throw new SQLiteException("Failed initialize RepairKit.");

        mIntegrityFlags = nativeIntegrityFlags(mNativePtr);
        mMasterInfo = master;
        mLeafInfo = leaf;
    }

    /**
     * Returns the progress callback set previously.
     *
     * @return the progress callback object
     */
    public Callback getCallback() {
        return mCallback;
    }

    /**
     * Set the progress callback to be called during {@link #output(SQLiteDatabase, int)}.
     *
     * @param callback the callback object to be set
     */
    public void setCallback(Callback callback) {
        mCallback = callback;
    }

    /**
     * Close corrupted database and release all resources. Do not call any methods
     * after this method is called.
     */
    public void release() {
        if (mLeafInfo != null) {
            mLeafInfo.release();
            mLeafInfo = null;
        }

        if (mMasterInfo != null) {
            mMasterInfo.release();
            mMasterInfo = null;
        }

        if (mNativePtr != 0) {
            nativeFini(mNativePtr);
            mNativePtr = 0;
        }
    }

    /**
     * Parse corrupted database and output its content to {@code db}.
     *
     * <p>This method does not return until repairing is finished. Don't
     * call it in the main thread or it will cause ANR.</p>
     *
     * @param db    destination database to be written
     * @param flags flags affects repair behavior
     * @return      result code which is {@link #RESULT_OK}, {@link #RESULT_CANCELED}
     *              or {@link #RESULT_FAILED}.
     */
    public int output(SQLiteDatabase db, int flags) {
        if (mNativePtr == 0)
            throw new IllegalArgumentException();

        long masterPtr = (mMasterInfo == null) ? 0 : mMasterInfo.mMasterPtr;
        long leafPtr = (mLeafInfo == null) ? 0 : mLeafInfo.mLeafPtr;

        long dbPtr = db.acquireNativeConnectionHandle("repair", false, false);
        int ret = nativeOutput(mNativePtr, dbPtr, masterPtr, leafPtr, flags);
        db.releaseNativeConnection(dbPtr, null);
        mCurrentCursor = null;

        mIntegrityFlags = nativeIntegrityFlags(mNativePtr);
        return ret;
    }

    /**
     * Parse corrupted database and output its content to {@code db}.
     *
     * <p>This method does not return until repairing is finished. Don't
     * call it in the main thread or it will cause ANR.</p>
     *
     * @param db    destination database to be written
     * @param flags flags affects repair behavior
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none
     * @return      result code which is {@link #RESULT_OK}, {@link #RESULT_CANCELED}
     *              or {@link #RESULT_FAILED}.
     */
    public int output(SQLiteDatabase db, int flags, CancellationSignal cancellationSignal) {
        if (cancellationSignal.isCanceled())
            return RESULT_CANCELED;

        cancellationSignal.setOnCancelListener(this);
        int result = output(db, flags);
        cancellationSignal.setOnCancelListener(null);

        return result;
    }

    @Override
    public void onCancel() {
        if (mNativePtr == 0)
            return;
        nativeCancel(mNativePtr);
    }

    @SuppressWarnings("unused")
    private int onProgress(String table, int root, long cursorPtr) {
        if (mCallback == null)
            return RESULT_OK;

        if (mCurrentCursor == null) {
            mCurrentCursor = new RepairCursor();
        }

        mCurrentCursor.mPtr = cursorPtr;
        return mCallback.onProgress(table, root, mCurrentCursor);
    }

    /**
     * Retrieve whether KDF salt is corrupted.
     * @return {@code true} if corrupted
     */
    public boolean isSaltCorrupted() {
        return (mIntegrityFlags & INTEGRITY_KDF_SALT) == 0;
    }

    /**
     * Retrieve whether database header is corrupted.
     * @return {@code true} if corrupted
     */
    public boolean isHeaderCorrupted() {
        return (mIntegrityFlags & INTEGRITY_HEADER) == 0;
    }

    /**
     * Retrieve whether data is corrupted.
     * @return {@code true} if corrupted
     */
    public boolean isDataCorrupted() {
        return (mIntegrityFlags & INTEGRITY_DATA) == 0;
    }

    public static class Statistics {
        public final int totalPages;
        public final int validPages;
        public final int parsedPages;
        public final int damagedPages;
        Statistics(int[] st) {
            totalPages = st[0];
            validPages = st[1];
            parsedPages = st[2];
            damagedPages = st[3];
        }
    }
    public Statistics getStatistics() {
        if (mNativePtr == 0)
            throw new IllegalArgumentException();
        int[] stat = nativeGetStatistics(mNativePtr);
        return new Statistics(stat);
    }

    /**
     * Retrieve the last error message.
     * @return last error message
     */
    public static String lastError() {
        return nativeLastError();
    }

    @Override
    protected void finalize() throws Throwable {
        release();
        super.finalize();
    }

    /**
     * Class represent master info backed up from a {@link SQLiteDatabase}, which
     * can be used in recovery on database file with corrupted header.
     *
     * <p>Table filters can be applied while loading from file or creating an
     * empty {@code MasterInfo}.</p>
     *
     * @see RepairKit#RepairKit(String, byte[], SQLiteCipherSpec, MasterInfo)
     */
    public static class MasterInfo {
        long mMasterPtr;
        byte[] mKDFSalt;

        private MasterInfo(long ptr, byte[] salt) {
            mMasterPtr = ptr;
            mKDFSalt = salt;
        }

        /**
         * Create a {@code MasterInfo} object with no backup information.
         * The only scenario to call this method is to apply table filters
         * for recovery.
         *
         * @param tables array of table names to include in the filter
         * @return {@code MasterInfo} object just created
         */
        public static MasterInfo make(String[] tables) {
            long ptr = RepairKit.nativeMakeMaster(tables);
            if (ptr == 0)
                throw new SQLiteException("Cannot create MasterInfo.");

            return new MasterInfo(ptr, null);
        }

        /**
         * Load backup information from file and create a {@code MasterInfo}
         * object. Table filters can be applied while loading.
         *
         * @param path      path to the backup file
         * @param key       passphrase to the encrypted backup file, or null for
         *                  plain-text backup file
         * @param tables    array of table names to include in the filter
         * @return {@code MasterInfo} object just created
         */
        public static MasterInfo load(String path, byte[] key, String[] tables) {
            if (path == null)
                return make(tables);

            byte[] salt = new byte[16];
            long ptr = RepairKit.nativeLoadMaster(path, key, tables, salt);
            if (ptr == 0)
                throw new SQLiteException("Cannot create MasterInfo.");

            return new MasterInfo(ptr, salt);
        }

        /**
         * Save backup information from an opened {@link SQLiteDatabase} for later
         * corruption recovery.
         *
         * <p><strong>Call this method BEFORE database corruption.</strong> Backup
         * data will not change unless database schema is modified, by executing,
         * e.g. {@code CREATE TABLE}, {@code CREATE INDEX} or {@code ALTER TABLE}
         * statements.</p>
         *
         * <p>Best practise would be backing up when database just finished creation
         * or upgrade, which would be placed in
         * {@link com.tencent.wcdb.database.SQLiteOpenHelper#onCreate(SQLiteDatabase)}
         * or {@link com.tencent.wcdb.database.SQLiteOpenHelper#onUpgrade(SQLiteDatabase, int, int)}.</p>
         *
         * @param db    database to be backed up
         * @param path  output path to the backup file
         * @param key   passphrase to the backup file, or null for no encryption
         * @return      true if backup is finished successfully
         */
        public static boolean save(SQLiteDatabase db, String path, byte[] key) {
            long dbPtr = db.acquireNativeConnectionHandle("backupMaster", true, false);
            boolean ret = RepairKit.nativeSaveMaster(dbPtr, path, key);
            db.releaseNativeConnection(dbPtr, null);
            return ret;
        }

        /**
         * Close corrupted database and release all resources. This should be
         * called when recovery is finished. No further method calls on this
         * object after calling.
         */
        public void release() {
            if (mMasterPtr == 0) return;

            RepairKit.nativeFreeMaster(mMasterPtr);
            mMasterPtr = 0;
        }

        @Override
        protected void finalize() throws Throwable {
            release();
            super.finalize();
        }
    }

    public static class LeafInfo {
        long mLeafPtr;

        private LeafInfo(long leafPtr) {
            mLeafPtr = leafPtr;
        }

        public void release() {
            if (mLeafPtr == 0) return;

            nativeFreeLeaf(mLeafPtr);
            mLeafPtr = 0;
        }

        @Override
        protected void finalize() throws Throwable {
            release();
            super.finalize();
        }

        public static LeafInfo scan(SQLiteDatabase db, String[] tables, CancellationSignal cs) {
            final long[] cancelFlag = new long[1];

            if (cs != null) {
                cs.setOnCancelListener(new CancellationSignal.OnCancelListener() {
                    @Override
                    public void onCancel() {
                        if (cancelFlag[0] != 0)
                            nativeCancelScanLeaf(cancelFlag[0]);
                    }
                });
            }

            if (tables != null && tables.length == 0)
                tables = null;
            long dbPtr = db.acquireNativeConnectionHandle("scanLeaf", true, false);
            Exception ex = null;
            try {
                if (cs != null) {
                    cs.throwIfCanceled();
                }

                long leafPtr = nativeScanLeaf(dbPtr, tables, (cs == null) ? null : cancelFlag);
                if (leafPtr == 0)
                    throw new SQLiteException("Cannot scan leaf info.");
                return new LeafInfo(leafPtr);
            } catch (Exception e) {
                ex = e;
                throw e;
            } finally {
                if (cs != null) {
                    cs.setOnCancelListener(null);
                }
                db.releaseNativeConnection(dbPtr, ex);
            }
        }

        public void save(String path) throws IOException {
            if (mLeafPtr == 0) return;

            if (!nativeSaveLeaf(mLeafPtr, path))
                throw new IOException("Cannot save leaf info.");
        }

        public static LeafInfo load(String path) throws IOException {
            long leafPtr = nativeLoadLeaf(path);
            if (leafPtr == 0)
                throw new IOException("Cannot load leaf info.");
            return new LeafInfo(leafPtr);
        }
    }

    /**
     * Listener for repairing progress report.
     */
    public interface Callback {

        /**
         * Called whenever a row of data is read from the corrupted database file.
         *
         * <p>The implementation can use the {@link Cursor} object from the argument to access
         * the row data. Note that <b>no movement of the cursor is allowed</b>. The access is limited
         * to the current row when callback method is called.</p>
         *
         * <p>The implementation tells the library how to deal with the current row by returning
         * different values.
         * <ul>
         * <li>{@link #RESULT_OK} means output the row to the new database;</li>
         * <li>{@link #RESULT_CANCELED} means the repairing method should return immediately with
         * further iteration;</li>
         * <li>{@link #RESULT_IGNORE} means ignore the current row and proceed to the next.</li>
         * </ul></p>
         *
         * @param table  which table the row of data belong to
         * @param root   root page of the table
         * @param cursor cursor object to access the row data
         * @return result code to tell the further operations
         */
        int onProgress(String table, int root, Cursor cursor);
    }

    private static class RepairCursor extends AbstractCursor {

        @Override
        public int getCount() {
            throw new UnsupportedOperationException();
        }

        @Override
        public String[] getColumnNames() {
            throw new UnsupportedOperationException();
        }

        @Override
        public int getColumnCount() {
            return nativeGetColumnCount(mPtr);
        }

        @Override
        public int getType(int column) {
            return nativeGetType(mPtr, column);
        }

        @Override
        public String getString(int column) {
            return nativeGetString(mPtr, column);
        }

        @Override
        public short getShort(int column) {
            return (short) getLong(column);
        }

        @Override
        public int getInt(int column) {
            return (int) getLong(column);
        }

        @Override
        public long getLong(int column) {
            return nativeGetLong(mPtr, column);
        }

        @Override
        public float getFloat(int column) {
            return (float) getDouble(column);
        }

        @Override
        public double getDouble(int column) {
            return nativeGetDouble(mPtr, column);
        }

        @Override
        public byte[] getBlob(int column) {
            return nativeGetBlob(mPtr, column);
        }

        @Override
        public boolean isNull(int column) {
            return getType(column) == FIELD_TYPE_NULL;
        }

        long mPtr;
        private static native int nativeGetColumnCount(long ptr);
        private static native int nativeGetType(long ptr, int column);
        private static native long nativeGetLong(long ptr, int column);
        private static native double nativeGetDouble(long ptr, int column);
        private static native String nativeGetString(long ptr, int column);
        private static native byte[] nativeGetBlob(long ptr, int column);
    }

    private static native long nativeInit(String path, byte[] key, SQLiteCipherSpec cipherSpec,
                                          byte[] salt);
    private static native void nativeFini(long rkPtr);
    private native int nativeOutput(long rkPtr, long dbPtr, long masterPtr, long leafPtr, int flags);
    private static native void nativeCancel(long rkPtr);
    private static native int nativeIntegrityFlags(long rkPtr);
    private static native String nativeLastError();
    private static native int[] nativeGetStatistics(long rkPtr);
    private static native long nativeMakeMaster(String[] tables);
    private static native boolean nativeSaveMaster(long dbPtr, String path, byte[] key);
    private static native long nativeLoadMaster(String path, byte[] key, String[] tables,
                                                byte[] outSalt);
    private static native void nativeFreeMaster(long masterPtr);

    private static native long nativeScanLeaf(long dbPtr, String[] tables, long[] outCancelFlag);
    private static native void nativeCancelScanLeaf(long cancelFlagPtr);
    private static native boolean nativeSaveLeaf(long leafPtr, String path);
    private static native long nativeLoadLeaf(String path);
    private static native void nativeFreeLeaf(long leafPtr);
}

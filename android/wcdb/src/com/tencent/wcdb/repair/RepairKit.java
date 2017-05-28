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

import com.tencent.wcdb.database.SQLiteCipherSpec;
import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.database.SQLiteException;


/**
 * Database repair toolkit to parse a corrupted database file and
 * write its content to another (newly created) database.
 */
public class RepairKit {

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


    private static final int INTEGRITY_HEADER = 0x0001;
    private static final int INTEGRITY_DATA = 0x0002;
    private static final int INTEGRITY_KDF_SALT = 0x0004;

    private long mNativePtr;
    private int mIntegrityFlags;
    private MasterInfo mMasterInfo;


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
        if (path == null)
            throw new IllegalArgumentException();

        mNativePtr = nativeInit(path, key, cipherSpec, (master == null) ? null : master.mKDFSalt);
        if (mNativePtr == 0)
            throw new SQLiteException("Failed initialize RepairKit.");

        mIntegrityFlags = nativeIntegrityFlags(mNativePtr);
        mMasterInfo = master;
    }

    /**
     * Close corrupted database and release all resources. Do not call any methods
     * after this method is called.
     */
    public void release() {
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
     * @return      {@code true} if at least one row is successfully repaired
     */
    public boolean output(SQLiteDatabase db, int flags) {
        if (mNativePtr == 0)
            throw new IllegalArgumentException();

        long masterPtr = (mMasterInfo == null) ? 0 : mMasterInfo.mMasterPtr;

        long dbPtr = db.acquireNativeConnectionHandle("repair", false, false);
        boolean ret = nativeOutput(mNativePtr, dbPtr, masterPtr, flags);
        db.releaseNativeConnection(dbPtr, null);

        mIntegrityFlags = nativeIntegrityFlags(mNativePtr);
        return ret;
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
        private long mMasterPtr;
        private byte[] mKDFSalt;

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

    private static native long nativeInit(String path, byte[] key, SQLiteCipherSpec cipherSpec, byte[] salt);
    private static native void nativeFini(long rkPtr);
    private static native boolean nativeOutput(long rkPtr, long dbPtr, long masterPtr, int flags);
    private static native int nativeIntegrityFlags(long rkPtr);
    private static native String nativeLastError();
    private static native long nativeMakeMaster(String[] tables);
    private static native boolean nativeSaveMaster(long dbPtr, String path, byte[] key);
    private static native long nativeLoadMaster(String path, byte[] key, String[] tables, byte[] outSalt);
    private static native void nativeFreeMaster(long masterPtr);
}

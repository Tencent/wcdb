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

import com.tencent.wcdb.SQLException;
import com.tencent.wcdb.database.SQLiteCipherSpec;
import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.database.SQLiteException;


public class RepairKit {

    public static final int FLAG_NO_CREATE_TABLES = 0x0001;
    public static final int FLAG_ALL_TABLES = 0x0002;

    private static final int INTEGRITY_HEADER = 0x0001;
    private static final int INTEGRITY_DATA = 0x0002;
    private static final int INTEGRITY_KDF_SALT = 0x0004;

    private long mNativePtr;
    private int mIntegrityFlags;
    private MasterInfo mMasterInfo;

    public RepairKit(String path, byte[] key, SQLiteCipherSpec cipherSpec, MasterInfo master) {
        if (path == null)
            throw new IllegalArgumentException();

        mNativePtr = nativeInit(path, key, cipherSpec, (master == null) ? null : master.mKDFSalt);
        if (mNativePtr == 0)
            throw new SQLException("Failed initialize RepairKit.");

        mIntegrityFlags = nativeIntegrityFlags(mNativePtr);
        mMasterInfo = master;
    }

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

    public boolean isSaltCorrupted() {
        return (mIntegrityFlags & INTEGRITY_KDF_SALT) == 0;
    }

    public boolean isHeaderCorrupted() {
        return (mIntegrityFlags & INTEGRITY_HEADER) == 0;
    }

    public boolean isDataCorrupted() {
        return (mIntegrityFlags & INTEGRITY_DATA) == 0;
    }

    public static String lastError() {
        return nativeLastError();
    }

    @Override
    protected void finalize() throws Throwable {
        release();
        super.finalize();
    }

    public static class MasterInfo {
        private long mMasterPtr;
        private byte[] mKDFSalt;

        private MasterInfo(long ptr, byte[] salt) {
            mMasterPtr = ptr;
            mKDFSalt = salt;
        }

        public static MasterInfo make(String[] tables) {
            long ptr = RepairKit.nativeMakeMaster(tables);
            if (ptr == 0)
                throw new SQLiteException("Cannot create MasterInfo.");

            return new MasterInfo(ptr, null);
        }

        public static MasterInfo load(String path, byte[] key, String[] tables) {
            byte[] salt = new byte[16];

            long ptr = RepairKit.nativeLoadMaster(path, key, tables, salt);
            if (ptr == 0)
                throw new SQLiteException("Cannot create MasterInfo.");

            return new MasterInfo(ptr, salt);
        }

        public static boolean save(SQLiteDatabase db, String path, byte[] key) {
            long dbPtr = db.acquireNativeConnectionHandle("backupMaster", true, false);
            boolean ret = RepairKit.nativeSaveMaster(dbPtr, path, key);
            db.releaseNativeConnection(dbPtr, null);
            return ret;
        }

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

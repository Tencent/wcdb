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

import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.database.SQLiteException;

import java.util.Arrays;


/**
 * Data backup toolkit based on table traversal.
 */
public class BackupKit {

	/*package*/ static final String TAG = "WCDB.DBBackup";

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

	/**
	 * Flag indicates no encryption is used on backup data.
	 */
	public static final int FLAG_NO_CIPHER = 0x0001;

	/**
	 * Flag indicates no compression is used on backup data.
	 */
	public static final int FLAG_NO_COMPRESS = 0x0002;

	/**
	 * Flag indicates when database corruption detected, try to read more data if possible
	 * rather than return failure immediately.
	 */
	public static final int FLAG_FIX_CORRUPTION = 0x0004;

	/**
	 * Flag indicates no "CREATE TABLE" statement should be output to backup file.
	 * When backup file created this way is being recovered, it won't execute
	 * "CREATE TABLE" or "CREATE INDEX" statement.
	 */
	public static final int FLAG_NO_CREATE_TABLE = 0x0008;

	/**
	 * Flag indicates incremental backup is performed. Incremental backup appends new data
	 * to the end of the backup file instead of overwriting. Use this flag with conditional
	 * backup to implement incremental backup.
	 *
	 * @see #BackupKit(SQLiteDatabase, String, byte[], int, String[])
	 */
	public static final int FLAG_INCREMENTAL = 0x0010;
	
	private long mNativePtr;
	private SQLiteDatabase mDB;
	private String[] mTableDesc;
	private int mStatementCount;
	private String mLastError;

	/**
	 * Create and initialize a backup task.
	 *
	 * <p>Conditional or incremental backup can be implemented by passing non-null
	 * to tableDesc argument.</p>
	 *
	 * <p>{@code tableDesc} should be array with length of a multiple of 2. The elements with
	 * even index are table name white-list to be backed up. Tables not listed are ignored.
	 * The elements with odd index are condition expression used in WHERE-clause when
	 * data to be backed up is selected, which can be null to indicate unconditional selection.</p>
	 *
	 * <p>For example, the array listed below tells to
	 * backup only the whole {@code contact} table and {@code message} table with
	 * column {@code id} greater than 100.
	 * </p>
	 *
	 * <pre><code>
	 *     new String[] {
	 *         "contact", null,
	 *         "message", "id > 100",
	 *     };
	 * </code></pre>
	 *
	 * @param db		database to be backed up.
	 * @param outPath	path to the output backup file.
	 * @param key		key for backup file encryption. Pass null to skip encryption.
	 * @param flags		flags to the backup operation.
	 * @param tableDesc table description for conditional backup.
	 * @throws SQLiteException when backup operation cannot be initialized.
	 * @throws IllegalArgumentException when outPath is null.
	 */
	public BackupKit(SQLiteDatabase db, String outPath, byte[] key, int flags, String[] tableDesc)
			throws SQLiteException {
		mDB = db;
		mLastError = null;
		mTableDesc = Arrays.copyOf(tableDesc, tableDesc.length);
		if (outPath == null)
			throw new IllegalArgumentException();
		mNativePtr = nativeInit(outPath, key, flags);
		if (mNativePtr == 0)
			throw new SQLiteException("Failed initialize backup context.");
	}

	/**
	 * Run backup task.
	 *
	 * <p><i>Note: this method will not return until backup is finished or cancelled.
	 * Do not call this method directly on the main thread or it will probably cause ANR.</i></p>
	 *
	 * @return	result code which is {@link #RESULT_OK}, {@link #RESULT_CANCELED} or {@link #RESULT_FAILED}.
	 */
	public int run() {
		if (mNativePtr == 0)
			throw new IllegalStateException("BackupKit not initialized.");

		long dbHandle = mDB.acquireNativeConnectionHandle("backup", false, false);
		int result = nativeRun(mNativePtr, dbHandle, mTableDesc);
		mDB.releaseNativeConnection(dbHandle, null);

		mStatementCount = nativeStatementCount(mNativePtr);
		mLastError = nativeLastError(mNativePtr);

		nativeFinish(mNativePtr);
		mNativePtr = 0;

		return result;
	}

	/**
	 * Retrieve total count of statements had been output during the last time {@link #run()}
	 * was called.
	 *
	 * @return	statement count had been output.
	 */
	public int statementCount() {
		return mStatementCount;
	}

	/**
	 * Cancel the current running backup operation.
	 *
	 * <p>This method can be called on threads other than the one called {@link #run()}
	 * Calling this method causes {@link #run()} to interrupt as quickly as possible and
	 * return {@link #RESULT_CANCELED}</p>
	 */
	public void cancel() {
		if (mNativePtr != 0)
			nativeCancel(mNativePtr);
	}

	/**
	 * Get the last error message.
	 *
	 * @return	last error message, can be null.
	 */
	public String lastError() {
		return mLastError;
	}

	/**
	 * Cleanup and release this backup task. No further operations should be done on this
	 * object after calling it.
	 */
	public void release() {
		if (mNativePtr != 0) {
			nativeFinish(mNativePtr);
			mNativePtr = 0;
		}
	}
	
	@Override
	protected void finalize() throws Throwable {
		release();
		super.finalize();
	}
	
	private static native long nativeInit(String outPath, byte[] key, int flags);
	private static native int nativeRun(long ctxHandle, long dbHandle, String[] tableDesc);
	private static native void nativeCancel(long ctxHandle);
	private static native void nativeFinish(long ctxHandle);
	private static native int nativeStatementCount(long ctxHandle);
	private static native String nativeLastError(long ctxHandle);
}

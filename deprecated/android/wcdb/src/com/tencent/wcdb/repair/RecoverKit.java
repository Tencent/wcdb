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
import com.tencent.wcdb.support.CancellationSignal;


/**
 * Data recovery toolkit that recover data backed up by {@link BackupKit}.
 */
public class RecoverKit implements CancellationSignal.OnCancelListener {

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
	
	private long mNativePtr;
	private SQLiteDatabase mDB;
	private int mFailedCount;
	private int mSuccessCount;
	private String mLastError;

	/**
	 * Create and initialize a recovery task.
	 *
	 * @param db		database where recovered data is written
	 * @param inPath	path to the backup file
	 * @param key		key to the encrypted backup file, or null for plain-text backup file.
	 * @throws SQLiteException when backup file cannot be opened.
	 */
	public RecoverKit(SQLiteDatabase db, String inPath, byte[] key)
			throws SQLiteException {
		mDB = db;
        mLastError = null;
		mNativePtr = nativeInit(inPath, key);
		if (mNativePtr == 0)
			throw new SQLiteException("Failed initialize recover context.");
	}

	/**
	 * Run recovery task.
	 *
	 * <p><i>Note: this method will not return until recovery is finished or cancelled.
	 * Do not call this method directly on the main thread or it will probably cause ANR.</i></p>
	 *
	 * @param fatal	whether or not to treat errors as fatal. When it's true and an error occurs,
	 *              recovery will end and return {@link #RESULT_CANCELED}.
	 * @return		result code which is {@link #RESULT_OK}, {@link #RESULT_CANCELED} or
	 * 				{@link #RESULT_FAILED}.
	 */
	public int run(final boolean fatal) {
		if (mNativePtr == 0)
			throw new IllegalStateException("RecoverKit not initialized.");

		long dbHandle = mDB.acquireNativeConnectionHandle("recover", false, false);

		int result = nativeRun(mNativePtr, dbHandle, fatal);
		mDB.releaseNativeConnection(dbHandle, null);

		mSuccessCount = nativeSuccessCount(mNativePtr);
		mFailedCount = nativeFailureCount(mNativePtr);
        mLastError = nativeLastError(mNativePtr);

        nativeFinish(mNativePtr);
        mNativePtr = 0;

		return result;
	}

	public int run(boolean fatal, CancellationSignal cancellationSignal) {
		if (cancellationSignal.isCanceled())
			return RESULT_CANCELED;

		cancellationSignal.setOnCancelListener(this);
		int result = run(fatal);
		cancellationSignal.setOnCancelListener(null);

		return result;
	}

	/**
	 * Retrieve total count of successful statements had been run during the last time
	 * {@link #run(boolean)} was called.
	 *
	 * @return	count of successful statements
	 */
	public int successCount() {
		return mSuccessCount;
	}

	/**
	 * Retrieve total count of statements had been failed during the last time
	 * {@link #run(boolean)} was called.
	 *
	 * @return	count of failed statements
	 */
	public int failureCount() {
		return mFailedCount;
	}

	/**
	 * Cancel the current running recovery operation.
	 *
	 * <p>This method can be called on threads other than the one called {@link #run(boolean)}
	 * Calling this method causes {@link #run(boolean)} to interrupt as quickly as possible and
	 * return {@link #RESULT_CANCELED}</p>
	 */
	@Override
	public void onCancel() {
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
	 * Cleanup and release this recovery task. No further operations should be done on this
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
	
	private static native long nativeInit(String inPath, byte[] key);
	private static native int nativeRun(long ctxHandle, long dbHandle, boolean fatal);
	private static native void nativeCancel(long ctxHandle);
	private static native void nativeFinish(long ctxHandle);
	private static native int nativeSuccessCount(long ctxHandle);
	private static native int nativeFailureCount(long ctxHandle);
	private static native String nativeLastError(long ctxHandle);
}

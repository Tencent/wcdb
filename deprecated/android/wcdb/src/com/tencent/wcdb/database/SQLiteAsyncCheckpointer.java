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

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.util.Pair;

import java.util.HashSet;


public class SQLiteAsyncCheckpointer implements SQLiteCheckpointListener, Handler.Callback {

    private Looper mLooper;
    private Handler mHandler;
    private boolean mUseDefaultLooper;
    private int mThreshold;
    private int mBlockingThreshold;
    private int mLastSyncMode;
    private final HashSet<Pair<SQLiteDatabase, String>> mPendingCheckpoints;

    private static final int DEFAULT_THRESHOLD = 100;
    private static final int DEFAULT_BLOCKING_THRESHOLD = 300;


    public SQLiteAsyncCheckpointer() {
        this(null, DEFAULT_THRESHOLD, DEFAULT_BLOCKING_THRESHOLD);
    }

    public SQLiteAsyncCheckpointer(Looper looper) {
        this(looper, DEFAULT_THRESHOLD, DEFAULT_BLOCKING_THRESHOLD);
    }

    public SQLiteAsyncCheckpointer(Looper looper, int threshold, int blockingThreshold) {
        mLooper = looper;
        mThreshold = threshold;
        mBlockingThreshold = blockingThreshold;
        mPendingCheckpoints = new HashSet<>();
    }

    @Override
    public void onAttach(SQLiteDatabase db) {
        if (mLooper == null) {
            mLooper = acquireDefaultLooper();
            mUseDefaultLooper = true;
        }

        mHandler = new Handler(mLooper, this);

        mLastSyncMode = db.getSynchronousMode();
        db.setSynchronousMode(SQLiteDatabase.SYNCHRONOUS_NORMAL);
    }

    @Override
    public void onWALCommit(SQLiteDatabase db, String dbName, int pages) {

        if (pages < mThreshold)
            return;
        boolean blockWriting = pages >= mBlockingThreshold;

        Pair<SQLiteDatabase, String> p = new Pair<>(db, dbName);

        boolean newTask;
        synchronized (mPendingCheckpoints) {
            newTask = mPendingCheckpoints.add(p);
        }
        if (!newTask)
            return;

        db.acquireReference();
        Message msg = mHandler.obtainMessage(0, blockWriting ? 1 : 0, 0, p);
        mHandler.sendMessage(msg);
    }

    @Override
    public void onDetach(SQLiteDatabase db) {
        db.setSynchronousMode(mLastSyncMode);
        mHandler = null;

        if (mUseDefaultLooper) {
            mLooper = null;
            releaseDefaultLooper();
            mUseDefaultLooper = false;
        }
    }

    @Override
    @SuppressWarnings("unchecked")
    public boolean handleMessage(Message msg) {
        Pair<SQLiteDatabase, String> p = (Pair<SQLiteDatabase, String>) msg.obj;
        SQLiteDatabase db = p.first;
        String dbName = p.second;
        boolean blockWriting = msg.arg1 != 0;

        try {
            long time = SystemClock.uptimeMillis();
            Pair<Integer, Integer> result = db.walCheckpoint(dbName, blockWriting);
            int walPages = result.first;
            int checkpointedPages = result.second;

            time = SystemClock.uptimeMillis() - time;
            onCheckpointResult(db, walPages, checkpointedPages, time);
        } finally {
            db.releaseReference();
        }

        synchronized (mPendingCheckpoints) {
            if (!mPendingCheckpoints.remove(p))
                throw new AssertionError("mPendingCheckpoints.remove(p)");
        }

        return true;
    }

    protected void onCheckpointResult(SQLiteDatabase db, int walPages, int checkpointedPages,
                                      long time) {
        // implemented by the derived class
    }


    private static Looper acquireDefaultLooper() {
        synchronized (gDefaultThreadLock) {
            if (gDefaultThreadRefCount++ == 0) {
                // Initialize default handler thread.
                if (gDefaultThread != null)
                    throw new AssertionError("gDefaultThread == null");

                gDefaultThread = new HandlerThread("WCDB.AsyncCheckpointer", 4);
                gDefaultThread.start();
            }

            return gDefaultThread.getLooper();
        }
    }

    private static void releaseDefaultLooper() {
        synchronized (gDefaultThreadLock) {
            if (--gDefaultThreadRefCount <= 0) {
                if (gDefaultThreadRefCount < 0)
                    throw new AssertionError("gDefaultThreadRefCount == 0");

                gDefaultThread.quit();
                gDefaultThread = null;
            }
        }
    }

    private static HandlerThread gDefaultThread;
    private static final Object gDefaultThreadLock = new Object();
    private static int gDefaultThreadRefCount = 0;
}

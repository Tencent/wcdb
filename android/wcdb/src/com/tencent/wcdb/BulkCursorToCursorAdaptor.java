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

package com.tencent.wcdb;

import android.os.Bundle;
import android.os.RemoteException;
import com.tencent.wcdb.support.Log;

/**
 * Adapts an {@link IBulkCursor} to a {@link Cursor} for use in the local process.
 *
 * {@hide}
 */
public final class BulkCursorToCursorAdaptor extends AbstractWindowedCursor {
    private static final String TAG = "BulkCursor";

    private SelfContentObserver mObserverBridge = new SelfContentObserver(this);
    private IBulkCursor mBulkCursor;
    private String[] mColumns;
    private boolean mWantsAllOnMoveCalls;
    private int mCount;

    /**
     * Initializes the adaptor.
     * Must be called before first use.
     */
    @SuppressWarnings("deprecation")
	public void initialize(BulkCursorDescriptor d) {
        mBulkCursor = d.cursor;
        mColumns = d.columnNames;
        mRowIdColumnIndex = DatabaseUtils.findRowIdColumnIndex(mColumns);
        mWantsAllOnMoveCalls = d.wantsAllOnMoveCalls;
        mCount = d.count;
        if (d.window != null) {
            setWindow(d.window);
        }
    }

    /**
     * Gets a SelfDataChangeOberserver that can be sent to a remote
     * process to receive change notifications over IPC.
     *
     * @return A SelfContentObserver hooked up to this Cursor
     */
    public IContentObserver getObserver() {
        // getContentObserver is defined as @hide in Android SDK, needs reflection to get access.
        try {
            return (IContentObserver) mObserverBridge.getClass().getMethod("getContentObserver")
                    .invoke(mObserverBridge);
        } catch (Exception e) {
            return null;
        }
    }

    private void throwIfCursorIsClosed() {
        if (mBulkCursor == null) {
            throw new StaleDataException("Attempted to access a cursor after it has been closed.");
        }
    }

    @Override
    public int getCount() {
        throwIfCursorIsClosed();
        return mCount;
    }

    @Override
    public boolean onMove(int oldPosition, int newPosition) {
        throwIfCursorIsClosed();

        try {
            // Make sure we have the proper window
            if (mWindow == null
                    || newPosition < mWindow.getStartPosition()
                    || newPosition >= mWindow.getStartPosition() + mWindow.getNumRows()) {
                setWindow(mBulkCursor.getWindow(newPosition));
            } else if (mWantsAllOnMoveCalls) {
                mBulkCursor.onMove(newPosition);
            }
        } catch (RemoteException ex) {
            // We tried to get a window and failed
            Log.e(TAG, "Unable to get window because the remote process is dead");
            return false;
        }

        // Couldn't obtain a window, something is wrong
        if (mWindow == null) {
            return false;
        }

        return true;
    }

    @Override
    public void deactivate() {
        // This will call onInvalidated(), so make sure to do it before calling release,
        // which is what actually makes the data set invalid.
        super.deactivate();

        if (mBulkCursor != null) {
            try {
                mBulkCursor.deactivate();
            } catch (RemoteException ex) {
                Log.w(TAG, "Remote process exception when deactivating");
            }
        }
    }
    
    @Override
    public void close() {
        super.close();

        if (mBulkCursor != null) {
            try {
                mBulkCursor.close();
            } catch (RemoteException ex) {
                Log.w(TAG, "Remote process exception when closing");
            } finally {
                mBulkCursor = null;
            }
        }
    }

    @Override
    public boolean requery() {
        throwIfCursorIsClosed();

        try {
            mCount = mBulkCursor.requery(getObserver());
            if (mCount != -1) {
                mPos = -1;
                closeWindow();

                // super.requery() will call onChanged. Do it here instead of relying on the
                // observer from the far side so that observers can see a correct value for mCount
                // when responding to onChanged.
                super.requery();
                return true;
            } else {
                deactivate();
                return false;
            }
        } catch (Exception ex) {
            Log.e(TAG, "Unable to requery because the remote process exception " + ex.getMessage());
            deactivate();
            return false;
        }
    }

    @Override
    public String[] getColumnNames() {
        throwIfCursorIsClosed();

        return mColumns;
    }

    @Override
    public Bundle getExtras() {
        throwIfCursorIsClosed();

        try {
            return mBulkCursor.getExtras();
        } catch (RemoteException e) {
            // This should never happen because the system kills processes that are using remote
            // cursors when the provider process is killed.
            throw new RuntimeException(e);
        }
    }

    @Override
    public Bundle respond(Bundle extras) {
        throwIfCursorIsClosed();

        try {
            return mBulkCursor.respond(extras);
        } catch (RemoteException e) {
            // the system kills processes that are using remote cursors when the provider process
            // is killed, but this can still happen if this is being called from the system process,
            // so, better to log and return an empty bundle.
            Log.w(TAG, "respond() threw RemoteException, returning an empty bundle.", e);
            return Bundle.EMPTY;
        }
    }

	@Override
	public void copyStringToBuffer(int arg0, android.database.CharArrayBuffer arg1)
	{
		// TODO Auto-generated method stub
	}

	@Override
	public void registerContentObserver(android.database.ContentObserver observer)
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void registerDataSetObserver(android.database.DataSetObserver observer)
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void unregisterContentObserver(android.database.ContentObserver observer)
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void unregisterDataSetObserver(android.database.DataSetObserver observer)
	{
		// TODO Auto-generated method stub
		
	}
}

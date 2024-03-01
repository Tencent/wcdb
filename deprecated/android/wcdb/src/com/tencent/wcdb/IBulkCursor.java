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
import android.os.IBinder;
import android.os.IInterface;
import android.os.RemoteException;

/**
 * This interface provides a low-level way to pass bulk cursor data across
 * both process and language boundaries. Application code should use the Cursor
 * interface directly.
 *
 * {@hide}
 */
public interface IBulkCursor extends IInterface  {
    /**
     * Gets a cursor window that contains the specified position.
     * The window will contain a range of rows around the specified position.
     */
    public CursorWindow getWindow(int position) throws RemoteException;

    /**
     * Notifies the cursor that the position has changed.
     * Only called when {@link #getWantsAllOnMoveCalls()} returns true.
     *
     * @param position The new position
     */
    public void onMove(int position) throws RemoteException;

    public void deactivate() throws RemoteException;

    public void close() throws RemoteException;

    public int requery(IContentObserver observer) throws RemoteException;

    Bundle getExtras() throws RemoteException;

    Bundle respond(Bundle extras) throws RemoteException;

    /* IPC constants */
    static final String descriptor = "android.content.IBulkCursor";

    static final int GET_CURSOR_WINDOW_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION;
    static final int DEACTIVATE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 1;
    static final int REQUERY_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 2;
    static final int ON_MOVE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 3;
    static final int GET_EXTRAS_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 4;
    static final int RESPOND_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 5;
    static final int CLOSE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 6;
}

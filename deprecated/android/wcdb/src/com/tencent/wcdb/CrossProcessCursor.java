/*
 * Copyright (C) 2008 The Android Open Source Project
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

/**
 * A cross process cursor is an extension of a {@link Cursor} that also supports
 * usage from remote processes.
 * <p>
 * The contents of a cross process cursor are marshalled to the remote process by
 * filling {@link CursorWindow} objects using {@link #fillWindow}.  As an optimization,
 * the cursor can provide a pre-filled window to use via {@link #getWindow} thereby
 * obviating the need to copy the data to yet another cursor window.
 */
public interface CrossProcessCursor extends Cursor {
    /**
     * Returns a pre-filled window that contains the data within this cursor.
     * <p>
     * In particular, the window contains the row indicated by {@link Cursor#getPosition}.
     * The window's contents are automatically scrolled whenever the current
     * row moved outside the range covered by the window.
     * </p>
     *
     * @return The pre-filled window, or null if none.
     */
    CursorWindow getWindow();

    /**
     * Copies cursor data into the window.
     * <p>
     * Clears the window and fills it with data beginning at the requested
     * row position until all of the data in the cursor is exhausted
     * or the window runs out of space.
     * </p><p>
     * The filled window uses the same row indices as the original cursor.
     * For example, if you fill a window starting from row 5 from the cursor,
     * you can query the contents of row 5 from the window just by asking it
     * for row 5 because there is a direct correspondence between the row indices
     * used by the cursor and the window.
     * </p><p>
     * The current position of the cursor, as returned by {@link #getPosition},
     * is not changed by this method.
     * </p>
     *
     * @param position The zero-based index of the first row to copy into the window.
     * @param window The window to fill.
     */
    void fillWindow(int position, CursorWindow window);

    /**
     * This function is called every time the cursor is successfully scrolled
     * to a new position, giving the subclass a chance to update any state it
     * may have.  If it returns false the move function will also do so and the
     * cursor will scroll to the beforeFirst position.
     * <p>
     * This function should be called by methods such as {@link #moveToPosition(int)},
     * so it will typically not be called from outside of the cursor class itself.
     * </p>
     *
     * @param oldPosition The position that we're moving from.
     * @param newPosition The position that we're moving to.
     * @return True if the move is successful, false otherwise.
     */
    boolean onMove(int oldPosition, int newPosition); 
}

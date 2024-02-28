/*
 * Copyright (C) 2011 The Android Open Source Project
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
 * limitations under the License
 */

package com.tencent.wcdb;

/**
 * Cursor wrapper that implements {@link CrossProcessCursor}.
 * <p>
 * If the wrapped cursor implements {@link CrossProcessCursor}, then the wrapper
 * delegates {@link #fillWindow}, {@link #getWindow()} and {@link #onMove} to it.
 * Otherwise, the wrapper provides default implementations of these methods that
 * traverse the contents of the cursor similar to {@link AbstractCursor#fillWindow}.
 * </p><p>
 * This wrapper can be used to adapt an ordinary {@link Cursor} into a
 * {@link CrossProcessCursor}.
 * </p>
 */
public class CrossProcessCursorWrapper extends CursorWrapper implements CrossProcessCursor {
    /**
     * Creates a cross process cursor wrapper.
     * @param cursor The underlying cursor to wrap.
     */
    public CrossProcessCursorWrapper(Cursor cursor) {
        super(cursor);
    }

    @Override
    public void fillWindow(int position, CursorWindow window) {
        if (mCursor instanceof CrossProcessCursor) {
            final CrossProcessCursor crossProcessCursor = (CrossProcessCursor)mCursor;
            crossProcessCursor.fillWindow(position, window);
            return;
        }

        DatabaseUtils.cursorFillWindow(mCursor, position, window);
    }

    @Override
    public CursorWindow getWindow() {
        if (mCursor instanceof CrossProcessCursor) {
            final CrossProcessCursor crossProcessCursor = (CrossProcessCursor)mCursor;
            return crossProcessCursor.getWindow();
        }

        return null;
    }

    @Override
    public boolean onMove(int oldPosition, int newPosition) {
        if (mCursor instanceof CrossProcessCursor) {
            final CrossProcessCursor crossProcessCursor = (CrossProcessCursor)mCursor;
            return crossProcessCursor.onMove(oldPosition, newPosition);
        }

        return true;
    }
}

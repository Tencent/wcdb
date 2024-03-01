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

import java.util.Iterator;

/**
 * Does a join on two cursors using the specified columns. The cursors must already
 * be sorted on each of the specified columns in ascending order. This joiner only
 * supports the case where the tuple of key column values is unique.
 * <p>
 * Typical usage:
 *
 * <pre>
 * CursorJoiner joiner = new CursorJoiner(cursorA, keyColumnsofA, cursorB, keyColumnsofB);
 * for (CursorJointer.Result joinerResult : joiner) {
 *     switch (joinerResult) {
 *         case LEFT:
 *             // handle case where a row in cursorA is unique
 *             break;
 *         case RIGHT:
 *             // handle case where a row in cursorB is unique
 *             break;
 *         case BOTH:
 *             // handle case where a row with the same key is in both cursors
 *             break;
 *     }
 * }
 * </pre>
 */
public final class CursorJoiner
        implements Iterator<CursorJoiner.Result>, Iterable<CursorJoiner.Result> {
    private Cursor mCursorLeft;
    private Cursor mCursorRight;
    private boolean mCompareResultIsValid;
    private Result mCompareResult;
    private int[] mColumnsLeft;
    private int[] mColumnsRight;
    private String[] mValues;

    /**
     * The result of a call to next().
     */
    public enum Result {
        /** The row currently pointed to by the left cursor is unique */
        RIGHT,
        /** The row currently pointed to by the right cursor is unique */
        LEFT,
        /** The rows pointed to by both cursors are the same */
        BOTH
    }

    /**
     * Initializes the CursorJoiner and resets the cursors to the first row. The left and right
     * column name arrays must have the same number of columns.
     * @param cursorLeft The left cursor to compare
     * @param columnNamesLeft The column names to compare from the left cursor
     * @param cursorRight The right cursor to compare
     * @param columnNamesRight The column names to compare from the right cursor
     */
    public CursorJoiner(
            Cursor cursorLeft, String[] columnNamesLeft,
            Cursor cursorRight, String[] columnNamesRight) {
        if (columnNamesLeft.length != columnNamesRight.length) {
            throw new IllegalArgumentException(
                    "you must have the same number of columns on the left and right, "
                            + columnNamesLeft.length + " != " + columnNamesRight.length);
        }

        mCursorLeft = cursorLeft;
        mCursorRight = cursorRight;

        mCursorLeft.moveToFirst();
        mCursorRight.moveToFirst();

        mCompareResultIsValid = false;

        mColumnsLeft = buildColumnIndiciesArray(cursorLeft, columnNamesLeft);
        mColumnsRight = buildColumnIndiciesArray(cursorRight, columnNamesRight);

        mValues = new String[mColumnsLeft.length * 2];
    }

    public Iterator<Result> iterator() {
        return this;
    }

    /**
     * Lookup the indicies of the each column name and return them in an array.
     * @param cursor the cursor that contains the columns
     * @param columnNames the array of names to lookup
     * @return an array of column indices
     */
    private int[] buildColumnIndiciesArray(Cursor cursor, String[] columnNames) {
        int[] columns = new int[columnNames.length];
        for (int i = 0; i < columnNames.length; i++) {
            columns[i] = cursor.getColumnIndexOrThrow(columnNames[i]);
        }
        return columns;
    }

    /**
     * Returns whether or not there are more rows to compare using next().
     * @return true if there are more rows to compare
     */
    public boolean hasNext() {
        if (mCompareResultIsValid) {
            switch (mCompareResult) {
                case BOTH:
                    return !mCursorLeft.isLast() || !mCursorRight.isLast();

                case LEFT:
                    return !mCursorLeft.isLast() || !mCursorRight.isAfterLast();

                case RIGHT:
                    return !mCursorLeft.isAfterLast() || !mCursorRight.isLast();

                default:
                    throw new IllegalStateException("bad value for mCompareResult, "
                            + mCompareResult);
            }
        } else {
            return !mCursorLeft.isAfterLast() || !mCursorRight.isAfterLast();
        }
    }

    /**
     * Returns the comparison result of the next row from each cursor. If one cursor
     * has no more rows but the other does then subsequent calls to this will indicate that
     * the remaining rows are unique.
     * <p>
     * The caller must check that hasNext() returns true before calling this.
     * <p>
     * Once next() has been called the cursors specified in the result of the call to
     * next() are guaranteed to point to the row that was indicated. Reading values
     * from the cursor that was not indicated in the call to next() will result in
     * undefined behavior.
     * @return LEFT, if the row pointed to by the left cursor is unique, RIGHT
     *   if the row pointed to by the right cursor is unique, BOTH if the rows in both
     *   cursors are the same.
     */
    public Result next() {
        if (!hasNext()) {
            throw new IllegalStateException("you must only call next() when hasNext() is true");
        }
        incrementCursors();
        if (BuildConfig.DEBUG && !hasNext())
            throw new AssertionError("hasNext()");

        boolean hasLeft = !mCursorLeft.isAfterLast();
        boolean hasRight = !mCursorRight.isAfterLast();

        if (hasLeft && hasRight) {
            populateValues(mValues, mCursorLeft, mColumnsLeft, 0 /* start filling at index 0 */);
            populateValues(mValues, mCursorRight, mColumnsRight, 1 /* start filling at index 1 */);
            switch (compareStrings(mValues)) {
                case -1:
                    mCompareResult = Result.LEFT;
                    break;
                case 0:
                    mCompareResult = Result.BOTH;
                    break;
                case 1:
                    mCompareResult = Result.RIGHT;
                    break;
            }
        } else if (hasLeft) {
            mCompareResult = Result.LEFT;
        } else  {
            if (BuildConfig.DEBUG && !hasRight)
                throw new AssertionError("hasRight");
            mCompareResult = Result.RIGHT;
        }
        mCompareResultIsValid = true;
        return mCompareResult;
    }

    public void remove() {
        throw new UnsupportedOperationException("not implemented");
    }

    /**
     * Reads the strings from the cursor that are specifed in the columnIndicies
     * array and saves them in values beginning at startingIndex, skipping a slot
     * for each value. If columnIndicies has length 3 and startingIndex is 1, the
     * values will be stored in slots 1, 3, and 5.
     * @param values the String[] to populate
     * @param cursor the cursor from which to read
     * @param columnIndicies the indicies of the values to read from the cursor
     * @param startingIndex the slot in which to start storing values, and must be either 0 or 1.
     */
    private static void populateValues(String[] values, Cursor cursor, int[] columnIndicies,
            int startingIndex) {
        if (BuildConfig.DEBUG && !(startingIndex == 0 || startingIndex == 1))
            throw new AssertionError("startingIndex == 0 || startingIndex == 1");
        for (int i = 0; i < columnIndicies.length; i++) {
            values[startingIndex + i*2] = cursor.getString(columnIndicies[i]);
        }
    }

    /**
     * Increment the cursors past the rows indicated in the most recent call to next().
     * This will only have an affect once per call to next().
     */
    private void incrementCursors() {
        if (mCompareResultIsValid) {
            switch (mCompareResult) {
                case LEFT:
                    mCursorLeft.moveToNext();
                    break;
                case RIGHT:
                    mCursorRight.moveToNext();
                    break;
                case BOTH:
                    mCursorLeft.moveToNext();
                    mCursorRight.moveToNext();
                    break;
            }
            mCompareResultIsValid = false;
        }
    }

    /**
     * Compare the values. Values contains n pairs of strings. If all the pairs of strings match
     * then returns 0. Otherwise returns the comparison result of the first non-matching pair
     * of values, -1 if the first of the pair is less than the second of the pair or 1 if it
     * is greater.
     * @param values the n pairs of values to compare
     * @return -1, 0, or 1 as described above.
     */
    private static int compareStrings(String... values) {
        if ((values.length % 2) != 0) {
            throw new IllegalArgumentException("you must specify an even number of values");
        }

        for (int index = 0; index < values.length; index+=2) {
            if (values[index] == null) {
                if (values[index+1] == null) continue;
                return -1;
            }

            if (values[index+1] == null) {
                return 1;
            }

            int comp = values[index].compareTo(values[index+1]);
            if (comp != 0) {
                return comp < 0 ? -1 : 1;
            }
        }

        return 0;
    }
}

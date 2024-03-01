/*
 * Copyright (C) 2012 The Android Open Source Project
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

import android.os.Parcel;
import android.os.Parcelable;

/**
 * Describes the properties of a {@link CursorToBulkCursorAdaptor} that are
 * needed to initialize its {@link BulkCursorToCursorAdaptor} counterpart on the client's end.
 *
 * {@hide}
 */
public final class BulkCursorDescriptor implements Parcelable {
    public static final Parcelable.Creator<BulkCursorDescriptor> CREATOR =
            new Parcelable.Creator<BulkCursorDescriptor>() {
        @Override
        public BulkCursorDescriptor createFromParcel(Parcel in) {
            BulkCursorDescriptor d = new BulkCursorDescriptor();
            d.readFromParcel(in);
            return d;
        }

        @Override
        public BulkCursorDescriptor[] newArray(int size) {
            return new BulkCursorDescriptor[size];
        }
    };

    public IBulkCursor cursor;
    public String[] columnNames;
    public boolean wantsAllOnMoveCalls;
    public int count;
    public CursorWindow window;

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeStrongBinder(cursor.asBinder());
        out.writeStringArray(columnNames);
        out.writeInt(wantsAllOnMoveCalls ? 1 : 0);
        out.writeInt(count);
        if (window != null) {
            out.writeInt(1);
            window.writeToParcel(out, flags);
        } else {
            out.writeInt(0);
        }
    }

    /**
     * Read and return a String[] object from the parcel.
     * {@hide}
     */
    public final String[] readStringArray(Parcel in) {
        String[] array = null;

        int length = in.readInt();
        if (length >= 0)
        {
            array = new String[length];

            for (int i = 0 ; i < length ; i++)
            {
                array[i] = in.readString();
            }
        }

        return array;
    }
    public void readFromParcel(Parcel in) {
        cursor = BulkCursorNative.asInterface(in.readStrongBinder());
        columnNames = readStringArray(in);
        wantsAllOnMoveCalls = in.readInt() != 0;
        count = in.readInt();
        if (in.readInt() != 0) {
            window = CursorWindow.CREATOR.createFromParcel(in);
        }
    }
}

package com.tencent.wcdb.compat;

import android.os.CancellationSignal;

import com.tencent.wcdb.core.Handle;

public final class DatabaseUtils {
    private DatabaseUtils() {
    }

    public static long longForQuery(SQLiteDatabase db, String sql, Object[] selectionArgs) {
        try (SQLiteStatement stmt = db.compileStatement(sql)) {
            stmt.bindAllArgs(selectionArgs);
            return stmt.simpleQueryForLong();
        }
    }

    public static String stringForQuery(SQLiteDatabase db, String sql, Object[] selectionArgs) {
        try (SQLiteStatement stmt = db.compileStatement(sql)) {
            stmt.bindAllArgs(selectionArgs);
            return stmt.simpleQueryForString();
        }
    }

    static void bindCancellationSignal(Handle handle, CancellationSignal cancellationSignal) {
        if (cancellationSignal == null) return;

        final Handle.CancellationSignal cs = new Handle.CancellationSignal();
        cancellationSignal.setOnCancelListener(new CancellationSignal.OnCancelListener() {
            @Override
            public void onCancel() {
                cs.cancel();
            }
        });

        if (cancellationSignal.isCanceled()) {
            cs.cancel();
        }

        handle.attachCancellationSignal(cs);
    }
}

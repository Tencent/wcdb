package com.tencent.wcdb.room.db;

import android.arch.persistence.db.SupportSQLiteOpenHelper;

import com.tencent.wcdb.DatabaseErrorHandler;
import com.tencent.wcdb.database.SQLiteCipherSpec;

/**
 * Created by johnh on 2017/7/11.
 */

public class WCDBOpenHelperFactory implements SupportSQLiteOpenHelper.Factory {

    private byte[] mPassphrase;
    private SQLiteCipherSpec mCipherSpec;
    private DatabaseErrorHandler mErrorHandler;
    private boolean mWALMode;

    public WCDBOpenHelperFactory passphrase(byte[] value) {
        mPassphrase = value;
        return this;
    }

    public WCDBOpenHelperFactory cipherSpec(SQLiteCipherSpec cipher) {
        mCipherSpec = cipher;
        return this;
    }

    public WCDBOpenHelperFactory errorHandler(DatabaseErrorHandler handler) {
        mErrorHandler = handler;
        return this;
    }

    public WCDBOpenHelperFactory writeAheadLoggingEnabled(boolean wal) {
        mWALMode = wal;
        return this;
    }

    @Override
    public SupportSQLiteOpenHelper create(SupportSQLiteOpenHelper.Configuration configuration) {
        WCDBOpenHelper result =  new WCDBOpenHelper(configuration.context, configuration.name,
                mPassphrase, mCipherSpec, configuration.version, mErrorHandler,
                configuration.callback);
        result.setWriteAheadLoggingEnabled(mWALMode);
        return result;
    }
}

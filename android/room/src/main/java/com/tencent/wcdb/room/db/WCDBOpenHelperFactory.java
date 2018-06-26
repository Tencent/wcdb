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

package com.tencent.wcdb.room.db;

import android.arch.persistence.db.SupportSQLiteOpenHelper;

import com.tencent.wcdb.DatabaseErrorHandler;
import com.tencent.wcdb.database.SQLiteCipherSpec;

public class WCDBOpenHelperFactory implements SupportSQLiteOpenHelper.Factory {

    private byte[] mPassphrase;
    private SQLiteCipherSpec mCipherSpec;
    private boolean mWALMode;
    private boolean mAsyncCheckpoint;

    public WCDBOpenHelperFactory passphrase(byte[] value) {
        mPassphrase = value;
        return this;
    }

    public WCDBOpenHelperFactory cipherSpec(SQLiteCipherSpec cipher) {
        mCipherSpec = cipher;
        return this;
    }

    public WCDBOpenHelperFactory writeAheadLoggingEnabled(boolean wal) {
        mWALMode = wal;
        return this;
    }

    public WCDBOpenHelperFactory asyncCheckpointEnabled(boolean acp) {
        mAsyncCheckpoint = acp;
        return this;
    }

    @Override
    public SupportSQLiteOpenHelper create(SupportSQLiteOpenHelper.Configuration configuration) {
        WCDBOpenHelper result = new WCDBOpenHelper(configuration.context, configuration.name,
                mPassphrase, mCipherSpec, configuration.callback);
        result.setWriteAheadLoggingEnabled(mWALMode);
        result.setAsyncCheckpointEnabled(mAsyncCheckpoint);
        return result;
    }
}

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

package com.tencent.wcdb.sample.repairdb;

import android.content.Context;

import com.tencent.wcdb.DatabaseErrorHandler;
import com.tencent.wcdb.database.SQLiteCipherSpec;
import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.database.SQLiteOpenHelper;


public class DBHelper extends SQLiteOpenHelper {

    static final String DATABASE_NAME = "test-repair.db";
    static final int DATABASE_VERSION = 1;

    static final byte[] PASSPHRASE = "testkey".getBytes();

    // The test database is taken from SQLCipher test-suit.
    //
    // To be compatible with databases created by the official SQLCipher
    // library, a SQLiteCipherSpec must be specified with page size of
    // 1024 bytes.
    static final SQLiteCipherSpec CIPHER_SPEC = new SQLiteCipherSpec()
            .setPageSize(1024);


    // We don't want corrupted databases get deleted or renamed on this sample,
    // so use an empty DatabaseErrorHandler.
    static final DatabaseErrorHandler ERROR_HANDLER = new DatabaseErrorHandler() {
        @Override
        public void onCorruption(SQLiteDatabase dbObj) {
            // Do nothing
        }
    };

    public DBHelper(Context context) {

        super(context, DATABASE_NAME, PASSPHRASE, CIPHER_SPEC, null,
                DATABASE_VERSION, ERROR_HANDLER);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("CREATE TABLE t1(a,b);");

        // OPTIONAL: backup master info for corruption recovery.
        // However, we want to test recovery feature, so omit backup here.

        //RepairKit.MasterInfo.save(db, db.getPath() + "-mbak", PASSPHRASE);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        // Do nothing.
    }
}
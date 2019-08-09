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

package com.example.wcdb.sample.persistence;

import android.arch.persistence.db.SupportSQLiteDatabase;
import android.arch.persistence.room.Room;
import android.arch.persistence.room.RoomDatabase;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.example.wcdb.sample.persistence.entity.AppDatabase;
import com.example.wcdb.sample.persistence.entity.User;
import com.example.wcdb.sample.persistence.entity.UserDao;
import com.tencent.wcdb.database.SQLiteCipherSpec;
import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.extension.fts.MMFtsTokenizer;
import com.tencent.wcdb.room.db.WCDBDatabase;
import com.tencent.wcdb.room.db.WCDBOpenHelperFactory;

import java.util.List;

public class MainActivity extends AppCompatActivity {

    private static final SQLiteCipherSpec sCipherSpec = new SQLiteCipherSpec()
            .setPageSize(4096)
            .setKDFIteration(64000);

    private AppDatabase mAppDB;
    private UserDao mUsers;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mAppDB = Room.databaseBuilder(this, AppDatabase.class, "app-db")
                .allowMainThreadQueries()
                .openHelperFactory(new WCDBOpenHelperFactory()
                    .passphrase("passphrase".getBytes())
                    .cipherSpec(sCipherSpec)
                    .writeAheadLoggingEnabled(true)
                    .asyncCheckpointEnabled(true)
                )
                .addCallback(new RoomDatabase.Callback() {
                    @Override
                    public void onOpen(@NonNull SupportSQLiteDatabase db) {
                        SQLiteDatabase realDb = ((WCDBDatabase) db).getInnerDatabase();

                        realDb.addExtension(MMFtsTokenizer.EXTENSION);
                    }
                })
                .build();

        mUsers = mAppDB.userDao();

        User user = new User();
        user.firstName = "John";
        user.lastName = "He";
        mUsers.insert(user);

        user.firstName = "Sanhua";
        user.lastName = "Zhang";
        mUsers.insert(user);

        List<User> userList = mUsers.getAll();
    }
}

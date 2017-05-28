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

import android.database.Cursor;
import android.os.AsyncTask;
import android.support.v4.widget.SimpleCursorAdapter;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ListView;
import android.widget.Toast;

import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.database.SQLiteException;
import com.tencent.wcdb.database.SQLiteOpenHelper;
import com.tencent.wcdb.repair.RepairKit;
import com.tencent.wcdb.sample.repairdb.R;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Random;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "WCDB.RepairDBSample";

    private SQLiteDatabase mDB;
    private final SQLiteOpenHelper mDBHelper = new DBHelper(this);

    private ListView mListView;
    private SimpleCursorAdapter mAdapter;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Extract test database from assets.
        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                File dbFile = getDatabasePath(DBHelper.DATABASE_NAME);

                if (!dbFile.exists()) {
                    dbFile.getParentFile().mkdirs();

                    InputStream in = null;
                    OutputStream out = null;
                    try {
                        byte[] buffer = new byte[1024];
                        in = getAssets().open(DBHelper.DATABASE_NAME);
                        out = new FileOutputStream(dbFile);
                        int len;
                        while ((len = in.read(buffer)) != -1) {
                            out.write(buffer, 0, len);
                        }
                    } catch (IOException e) {
                        throw new RuntimeException(e);
                    } finally {
                        if (in != null) try { in.close(); } catch (IOException e) {}
                        if (out != null) try { out.close(); } catch (IOException e) {}
                    }
                }
                return null;
            }
        }.execute();

        setContentView(R.layout.activity_main);

        mListView = (ListView) findViewById(R.id.list);
        mAdapter = new SimpleCursorAdapter(this, R.layout.main_listitem, null,
                new String[] {"a", "b"},
                new int[] {R.id.list_tv_a, R.id.list_tv_b},
                0);

        mListView.setAdapter(mAdapter);

        findViewById(R.id.btn_init_db).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new AsyncTask<Void, Void, SQLiteException>() {
                    @Override
                    protected void onPreExecute() {
                        mAdapter.changeCursor(null);
                    }

                    @Override
                    protected SQLiteException doInBackground(Void... params) {
                        if (mDB != null && mDB.isOpen()) {
                            mDBHelper.close();
                            mDB = null;
                        }

                        try {
                            mDBHelper.setWriteAheadLoggingEnabled(true);
                            mDB = mDBHelper.getWritableDatabase();

                            // After successfully opened the database, backup its master info.
                            RepairKit.MasterInfo.save(mDB, mDB.getPath() + "-mbak", DBHelper.PASSPHRASE);
                        } catch (SQLiteException e) {
                            // Failed to open database, probably due to corruption.
                            mDB = null;
                            return e;
                        }

                        return null;
                    }

                    @Override
                    protected void onPostExecute(SQLiteException e) {
                        if (e == null) {
                            // Database is successfully opened, query and refresh ListView.
                            Cursor cursor = mDB.rawQuery("SELECT rowid as _id, a, b FROM t1;",
                                    null);
                            mAdapter.changeCursor(cursor);

                            Toast.makeText(MainActivity.this, "Database is successfully opened.",
                                    Toast.LENGTH_SHORT).show();
                        } else {
                            // Database could not be opened, show toast.
                            Toast.makeText(MainActivity.this, "Database cannot be opened, exception: "
                                    + e.getMessage(), Toast.LENGTH_LONG).show();
                        }
                    }
                }.execute();
            }
        });

        findViewById(R.id.btn_corrupt_db).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new AsyncTask<Void, Void, Exception>() {
                    @Override
                    protected void onPreExecute() {
                        mAdapter.changeCursor(null);
                    }

                    @Override
                    protected Exception doInBackground(Void... params) {
                        if (mDB != null && mDB.isOpen()) {
                            mDBHelper.close();
                            mDB = null;
                        }

                        // Write random noise to the first page to corrupt the database.
                        RandomAccessFile raf = null;
                        try {
                            File dbFile = getDatabasePath(DBHelper.DATABASE_NAME);
                            raf = new RandomAccessFile(dbFile, "rw");
                            byte[] buffer = new byte[1024];
                            new Random().nextBytes(buffer);
                            raf.seek(0);
                            raf.write(buffer);
                        } catch (IOException e) {
                            return e;
                        } finally {
                            if (raf != null) try { raf.close(); } catch (IOException e) {}
                        }

                        return null;
                    }

                    @Override
                    protected void onPostExecute(Exception e) {
                        if (e == null) {
                            Toast.makeText(MainActivity.this, "Database is now CORRUPTED!",
                                    Toast.LENGTH_SHORT).show();
                        } else {
                            Toast.makeText(MainActivity.this, "Unable to overwrite database: "
                                    + e.getMessage(), Toast.LENGTH_LONG).show();
                        }
                    }
                }.execute();
            }
        });

        findViewById(R.id.btn_repair_db).setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                new AsyncTask<Void, Void, SQLiteException>() {
                    @Override
                    protected void onPreExecute() {
                        mAdapter.changeCursor(null);
                    }

                    @Override
                    protected SQLiteException doInBackground(Void... params) {
                        if (mDB != null && mDB.isOpen()) {
                            mDBHelper.close();
                            mDB = null;
                        }

                        RepairKit.MasterInfo master = null;
                        File dbFile = getDatabasePath(DBHelper.DATABASE_NAME);
                        File masterFile = new File(dbFile.getPath() + "-mbak");
                        File newDbFile = getDatabasePath(DBHelper.DATABASE_NAME + "-recover");

                        if (masterFile.exists()) {
                            try {
                                master = RepairKit.MasterInfo.load(masterFile.getPath(),
                                        DBHelper.PASSPHRASE, null);
                            } catch (SQLiteException e) {
                                // Could not load master info. Maybe backup file itself corrupted?
                            }
                        }

                        RepairKit repair = null;
                        try {
                            repair = new RepairKit(
                                    dbFile.getPath(),       // corrupted database file
                                    DBHelper.PASSPHRASE,    // passphrase to the database
                                    DBHelper.CIPHER_SPEC,   // cipher spec to the database
                                    master                  // backup master info just loaded
                            );

                            if (newDbFile.exists())
                                newDbFile.delete();

                            SQLiteDatabase newDb = SQLiteDatabase.openOrCreateDatabase(newDbFile,
                                    DBHelper.PASSPHRASE, DBHelper.CIPHER_SPEC, null,
                                    DBHelper.ERROR_HANDLER);
                            boolean result = repair.output(newDb, 0);
                            if (!result) {
                                throw new SQLiteException("Repair returns false on output.");
                            }

                            newDb.setVersion(DBHelper.DATABASE_VERSION);
                            newDb.close();
                            repair.release();
                            repair = null;

                            if (!dbFile.delete() || !newDbFile.renameTo(dbFile))
                                throw new SQLiteException("Cannot rename database.");
                        } catch (SQLiteException e) {
                            return e;
                        } finally {
                            if (repair != null)
                                repair.release();
                        }

                        return null;
                    }

                    @Override
                    protected void onPostExecute(SQLiteException e) {
                        if (e == null) {
                            Toast.makeText(MainActivity.this, "Repair succeeded.", Toast.LENGTH_SHORT)
                                    .show();
                        } else {
                            Toast.makeText(MainActivity.this, "Repair failed: "
                                    + e.getMessage(), Toast.LENGTH_LONG).show();
                        }
                    }
                }.execute();
            }
        });
    }
}

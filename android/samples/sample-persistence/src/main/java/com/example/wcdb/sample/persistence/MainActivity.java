package com.example.wcdb.sample.persistence;

import android.arch.persistence.room.Room;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.example.wcdb.sample.persistence.entity.AppDatabase;
import com.tencent.wcdb.room.db.WCDBOpenHelperFactory;

public class MainActivity extends AppCompatActivity {

    private AppDatabase mAppDB;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mAppDB = Room.databaseBuilder(this, AppDatabase.class, "app-db")
                .allowMainThreadQueries()
                .openHelperFactory(new WCDBOpenHelperFactory()
                .passphrase("passphrase".getBytes())
                .writeAheadLoggingEnabled(true))
                .build();
    }
}

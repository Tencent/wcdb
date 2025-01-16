package com.tencent.wcdbtest

import android.content.Context
import com.tencent.wcdb.compat.DatabaseErrorHandler
import com.tencent.wcdb.compat.SQLiteCipherSpec
import com.tencent.wcdb.compat.SQLiteDatabase
import com.tencent.wcdb.compat.SQLiteOpenHelper
import com.tencent.wcdb.core.Database

// 表
const val TABLE_NAME_USER = "user"


// 列
const val COLUMN_USER_ID = "id"
const val COLUMN_USER_NAME = "name"
const val COLUMN_USER_AGE = "age"

class LocalWcdb2OpenHelper(
    context: Context?,
    name: String,
    password: ByteArray?,
    version: Int,
    errorHandler: DatabaseErrorHandler?
) : SQLiteOpenHelper(
    context,
    name,
    password,
    SQLiteCipherSpec().setSQLCipherVersion(Database.CipherVersion.version3.ordinal)
        .setPageSize(4096),
    version,
    errorHandler
) {
    override fun onCreate(db: SQLiteDatabase?) {
        // 创建一个表
        val sql = "CREATE TABLE IF NOT EXISTS $TABLE_NAME_USER(" +
                "$COLUMN_USER_ID INTEGER PRIMARY KEY AUTOINCREMENT, " +
                "$COLUMN_USER_NAME TEXT, " +
                "$COLUMN_USER_AGE INTEGER)"
        db?.execSQL(sql)
    }

    override fun onUpgrade(db: SQLiteDatabase?, oldVersion: Int, newVersion: Int) {
        // 这里简单删除表并重新创建
        db?.execSQL("DROP TABLE IF EXISTS $TABLE_NAME_USER")
        onCreate(db)
    }

}
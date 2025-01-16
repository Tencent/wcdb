package com.tencent.wcdbtest

import android.content.ContentValues
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.tencent.wcdb.compat.SQLiteDatabase.CONFLICT_ABORT
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext


class AwemeIMSDKTestActivity : ComponentActivity() {
    private var openHelper: LocalWcdb2OpenHelper? = null
    private var insertCount: Int = 0
    private val TAG = "jw"

    private var adapter: MyAdapter? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        startTest()
        val recyclerView: RecyclerView = findViewById(R.id.item_list)
        recyclerView.layoutManager = LinearLayoutManager(this)
        val dataList = ArrayList<String>()
        adapter = MyAdapter(dataList)
        recyclerView.adapter = adapter

        findViewById<Button>(R.id.btn_read).setOnClickListener {
            lifecycleScope.launch(Dispatchers.IO) {
                onReadDb()
            }
        }

        findViewById<Button>(R.id.btn_write_no_transaction).setOnClickListener {
            lifecycleScope.launch(Dispatchers.IO) {
                onWriteDbWithNoTransaction()
            }
        }

        findViewById<Button>(R.id.btn_write_single_transaction).setOnClickListener {
            lifecycleScope.launch(Dispatchers.IO) {
                onWriteDbWithSingleTransaction()
            }
        }

        findViewById<Button>(R.id.btn_write_nested_transaction).setOnClickListener {
            lifecycleScope.launch(Dispatchers.IO) {
                onWriteDbWithNestedTransaction()
            }
        }

        lifecycleScope.launch(Dispatchers.IO) {
            onInitDb()
            onReadDb()
        }

    }

    private fun startTest(){
        Log.w(TAG, "        阿弥陀佛")
        Log.w(TAG, "    阿弥陀佛  阿弥陀佛")
        Log.w(TAG, " 阿弥陀佛      阿弥陀佛")
        Log.w(TAG, "阿弥陀佛          阿弥陀佛")
        Log.w(TAG, " 阿弥陀佛      阿弥陀佛")
        Log.w(TAG, "    阿弥陀佛  阿弥陀佛")
        Log.w(TAG, "        阿弥陀佛")
    }

    private fun onInitDb() {
        if (openHelper != null) {
            Log.i(TAG, "数据库已经初始化，本次初始化跳过")
            return
        }
        Log.i(TAG, "开始初始化数据库")
        openHelper =
            LocalWcdb2OpenHelper(
                this@AwemeIMSDKTestActivity,
                "wcdb_demo_test.db",
                "123456".toByteArray(),
                2,
                null
            )
    }

    private suspend fun onReadDb(label: String = "无事务读测试") {
        Log.i(TAG, "开始进行$label")
        if (openHelper == null) {
            lifecycleScope.launch(Dispatchers.Main) {
                Log.w(TAG, "异常，未初始化数据库，测试之前需要先初始化数据库！")
                Toast.makeText(
                    this@AwemeIMSDKTestActivity,
                    "测试前需要先初始化DB",
                    Toast.LENGTH_SHORT
                ).show()
            }
            return
        }
        val dbHelper = openHelper!!
        val cursor = try {
            // 查询数据库中的记录数
            insertCount =
                dbHelper.writableDatabase.innerDB.getValueFromSQL("select COUNT(*) from $TABLE_NAME_USER")?.int
                    ?: 0
            dbHelper.writableDatabase.rawQuery("select * from $TABLE_NAME_USER", null)
        } catch (e: Exception) {
            null
        }
        if (cursor != null && !cursor.isClosed && cursor.moveToFirst()) {
            val nameIndex = cursor.getColumnIndex(COLUMN_USER_NAME)
            val dataList = ArrayList<String>()
            while (cursor.moveToNext()) {
                dataList.add(cursor.getString(nameIndex))
            }
            withContext(Dispatchers.Main) {
                adapter?.refreshData(dataList)
            }
        }
        cursor?.close()
    }

    private suspend fun onWriteDbWithNoTransaction(label: String = "无事务写测试") {
        Log.i(TAG, "开始进行$label")
        if (openHelper == null) {
            lifecycleScope.launch(Dispatchers.Main) {
                Log.w(TAG, "异常，未初始化数据库，测试之前需要先初始化数据库！")
                Toast.makeText(
                    this@AwemeIMSDKTestActivity,
                    "测试前需要先初始化DB",
                    Toast.LENGTH_SHORT
                ).show()
            }
            return
        }
        val dbHelper = openHelper!!
        val insertedStr = "insertCount=${insertCount++}_$label" + RandomTool.string()
        val insertRes = try {
            dbHelper.writableDatabase.insertWithOnConflict(
                TABLE_NAME_USER,
                null,
                ContentValues().apply {
                    put(
                        COLUMN_USER_NAME,
                        insertedStr
                    )
                    put(COLUMN_USER_AGE, RandomTool.getRandomInt())
                },
                CONFLICT_ABORT
            )
        } catch (e: Exception) {
            withContext(Dispatchers.Main) {
                Toast.makeText(
                    this@AwemeIMSDKTestActivity,
                    "测试标题：$label, 第${insertCount}次插入，遇到异常请检查",
                    Toast.LENGTH_SHORT
                ).show()
            }
            -1024
        }
        if (insertRes != -1024L) {
            withContext(Dispatchers.Main) {
                adapter?.insertItem(insertedStr)
                Toast.makeText(
                    this@AwemeIMSDKTestActivity,
                    "测试标题：$label, 第${insertCount}次插入，插入结果=$insertRes",
                    Toast.LENGTH_SHORT
                ).show()
            }
        }
    }

    private suspend fun onWriteDbWithSingleTransaction(label: String = "单个事务测试") {
        Log.i(TAG, "开始进行$label")
        if (openHelper == null) {
            lifecycleScope.launch(Dispatchers.Main) {
                Log.w(TAG, "异常，未初始化数据库，测试之前需要先初始化数据库！")
                Toast.makeText(
                    this@AwemeIMSDKTestActivity,
                    "测试前需要先初始化DB",
                    Toast.LENGTH_SHORT
                ).show()
            }
            return
        }
        val dbHelper = openHelper!!

        // 开启事务
        dbHelper.writableDatabase.beginTransactionNonExclusive()
        var success = false
        try {
            onWriteDbWithNoTransaction(label)
            success = true
        } catch (e: Exception) {
            withContext(Dispatchers.Main) {
                Toast.makeText(
                    this@AwemeIMSDKTestActivity,
                    "测试标题：$label，遇到异常请检查",
                    Toast.LENGTH_SHORT
                ).show()
            }
            success = false
        } finally {
            // 结束事务
            if (dbHelper.writableDatabase.inTransaction()) {
                if (success) {
                    // 标记事务成功
                    dbHelper.writableDatabase.setTransactionSuccessful()
                }
                dbHelper.writableDatabase.endTransaction()
            }
        }
    }

    private suspend fun onWriteDbWithNestedTransaction(label: String = "嵌套事务写测试") {
        Log.i(TAG, "开始进行$label")
        if (openHelper == null) {
            lifecycleScope.launch(Dispatchers.Main) {
                Log.w(TAG, "异常，未初始化数据库，测试之前需要先初始化数据库！")
                Toast.makeText(
                    this@AwemeIMSDKTestActivity,
                    "测试前需要先初始化DB",
                    Toast.LENGTH_SHORT
                ).show()
            }
            return
        }
        val dbHelper = openHelper!!

        // 开启外层事务
        dbHelper.writableDatabase.beginTransactionNonExclusive()

        // 嵌套事务一
        onWriteDbWithSingleTransaction(label+"_子事务1_")
        // 嵌套事务二
        onWriteDbWithSingleTransaction(label+"_子事务2_")

        // 结束外层事务
        dbHelper.writableDatabase.setTransactionSuccessful()
        dbHelper.writableDatabase.endTransaction()
    }
}

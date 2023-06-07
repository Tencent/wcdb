// Created by chenqiuwen on 2023/5/22.
//

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

package com.tencent.wcdbtest.sample;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.core.HandleOperation;
import com.tencent.wcdb.core.PreparedStatement;
import com.tencent.wcdb.core.Table;
import com.tencent.wcdb.core.Transaction;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdb.winq.Pragma;
import com.tencent.wcdb.winq.StatementInsert;
import com.tencent.wcdb.winq.StatementPragma;
import com.tencent.wcdb.winq.StatementSelect;
import com.tencent.wcdbtest.base.CRUDTestCase;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TestObject;

import org.junit.Test;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class SimpleSample extends CRUDTestCase {
    @Test
    public void sample() throws WCDBException {
        // Database对象可以在一个业务模块初始化的时候创建，无成本，然后跟随业务的其他逻辑的生命周期。
        Database database = new Database(currentDirectory + File.separator + "demoDatabase.sqlite3");
        // 这些配置都是在数据库实际使用的时候才生效，配置无成本
        database.setCipherKey("abc".getBytes(), 4096, Database.CipherVersion.version4);
        // 自定义配置，一般不需要
        database.setConfig("自定义配置名", new Database.Config() {
            @Override
            public void onInvocation(Handle handle) throws WCDBException {
                // Pragma secure_delete = true
                handle.execute(new StatementPragma().pragma(Pragma.secureDelete()).toValue(true));
            }
        });

        // 建表，不用判断表是否存在，底下会判断
        database.createTable("testTable", DBTestObject.INSTANCE);
        // Table是数据库、表名、ORM类的封装，建议跟随database的生命周期，简单的CRUD都用Table
        Table<TestObject> table = database.getTable("testTable", DBTestObject.INSTANCE);
        // 插入数据，这里创建的TestObject的id属性为0，因为配置了自增，插入之后会自动赋值id为最新rowid，还有insertOrReplaceObject、还有insertOrIgnoreObject接口
        table.insertObject(TestObject.createAutoIncrementObject(RandomTool.string()));
        // 批量插入，自动开事务
        table.insertObjects(RandomTool.autoIncrementTestCaseObjects(100));
        // 更新，可以用一个数据、一行数据、一个对象为单位去更新，后面还可以跟order，limit，offset参数
        table.updateValue("updateContent", DBTestObject.content, DBTestObject.id.eq(100));
        table.updateObject(TestObject.createObject(200, "updateContent2"), DBTestObject.allFields(),
                DBTestObject.id.eq(100).and(DBTestObject.content.eq("updateContent")));
        // 删除
        table.deleteObjects(DBTestObject.id.lt(10));
        table.deleteObjects(DBTestObject.id.order(Order.Desc), 10);
        // 读取
        TestObject object = table.getFirstObject(DBTestObject.id.eq(1));
        List<TestObject> objects = table.getAllObjects(DBTestObject.id.gt(100), DBTestObject.id.order(Order.Asc), 10);

        //执行事务
        database.runTransaction(new Transaction() {
            @Override
            public boolean insideTransaction(Handle handle) throws WCDBException {
                //上面那些接口都能用，也可以用一些高阶接口，直接拼sql对象，来使用全部的sql能力
                PreparedStatement insert = handle.getOrCreatePreparedStatement(new StatementInsert().insertInto("testTable").orReplace()
                        .columns(DBTestObject.allFields())
                        .valuesWithBindParameters(DBTestObject.allFields().length));
                for(int i = 0; i < 200; i++) {
                    TestObject obj = TestObject.createObject(i, RandomTool.string());
                    insert.reset();
                    insert.bindObject(obj, DBTestObject.allFields());
                    insert.step();
                    obj.id = (int)handle.getLastInsertedRowId();
                }
                // select count(*) from testTable
                int count = (int)handle.getValueFromStatement(new StatementSelect().select(Column.all().count()).from("testTable")).getInteger();

                PreparedStatement select = handle.getOrCreatePreparedStatement(new StatementSelect().select(DBTestObject.allFields())
                        .from("testTable").where(DBTestObject.content.notNull()));
                List<TestObject> result = new ArrayList<TestObject>();
                select.step();
                while(!select.isDone()) {
                    result.add(select.getOneObject(DBTestObject.allFields()));
                    select.step();
                }
                return true;//返回false回滚整个事务
            }
        });
    }
}

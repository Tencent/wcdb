// Created by qiuwenchen on 2024/2/22.
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

package com.tencent.wcdbtest.crud;

import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.core.Table;
import com.tencent.wcdb.fts.BuiltinTokenizer;
import com.tencent.wcdbtest.base.DBPinyinObject;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.DBTraditionalChineseObject;
import com.tencent.wcdbtest.base.DatabaseTestCase;
import com.tencent.wcdbtest.base.PinyinObject;
import com.tencent.wcdbtest.base.TestObject;
import com.tencent.wcdbtest.base.TraditionalChineseObject;
import com.tencent.wcdbtest.orm.testclass.DBFTS3TestObject;
import com.tencent.wcdbtest.orm.testclass.DBFTS5TestObject;
import com.tencent.wcdbtest.orm.testclass.DBMMICUTestObject;
import com.tencent.wcdbtest.orm.testclass.FTS3TestObject;
import com.tencent.wcdbtest.orm.testclass.FTS5TestObject;
import com.tencent.wcdbtest.orm.testclass.MMICUTestObject;

import org.junit.Assert;
import org.junit.Test;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;

public class FTSTest extends DatabaseTestCase {
    String tableName = "ftsTable";

    @Test
    public void testFTS3() {
        database.addTokenizer(BuiltinTokenizer.OneOrBinary);
        database.createVirtualTable(tableName, DBFTS3TestObject.INSTANCE);
        Table<FTS3TestObject> table = database.getTable(tableName, DBFTS3TestObject.INSTANCE);
        FTS3TestObject englishObject = new FTS3TestObject(1, "This is English test content");
        FTS3TestObject chineseObject = new FTS3TestObject(2, "这是中文测试内容");
        FTS3TestObject numericObject = new FTS3TestObject(1, "123456");
        FTS3TestObject symbolicObject = new FTS3TestObject(1, "abc..def");
        table.insertObjects(Arrays.asList(englishObject, chineseObject, numericObject, symbolicObject));

        // English
        List<FTS3TestObject> matchEnglishObjects = table.getAllObjects(DBFTS3TestObject.content.match("Engl*"));
        Assert.assertTrue(matchEnglishObjects.size() == 1 && matchEnglishObjects.get(0).content.equals(englishObject.content));

        // Chinese
        List<FTS3TestObject> matchChineseObjects = table.getAllObjects(DBFTS3TestObject.content.match("中文"));
        Assert.assertTrue(matchChineseObjects.size() == 1 && matchChineseObjects.get(0).content.equals(chineseObject.content));

        // Numeric
        List<FTS3TestObject> matchNumericObjects = table.getAllObjects(DBFTS3TestObject.content.match("123*"));
        Assert.assertTrue(matchNumericObjects.size() == 1 && matchNumericObjects.get(0).content.equals(numericObject.content));

        // Symbolic
        List<FTS3TestObject> matchSymbolicObjects = table.getAllObjects(DBFTS3TestObject.content.match("def*"));
        Assert.assertTrue(matchSymbolicObjects.size() == 1 && matchSymbolicObjects.get(0).content.equals(symbolicObject.content));
    }

    @Test
    public void testMMICU() {
        database.addTokenizer(BuiltinTokenizer.MMICU);
        database.createVirtualTable(tableName, DBMMICUTestObject.INSTANCE);
        Table<MMICUTestObject> table = database.getTable(tableName, DBMMICUTestObject.INSTANCE);
        MMICUTestObject englishObject = new MMICUTestObject(1, "This is English test content");
        MMICUTestObject chineseObject = new MMICUTestObject(2, "这是中文测试内容");
        MMICUTestObject numericObject = new MMICUTestObject(1, "123456");
        MMICUTestObject symbolicObject = new MMICUTestObject(1, "abc..def");
        MMICUTestObject emptyObject = new MMICUTestObject(3, "");
        table.insertObjects(Arrays.asList(englishObject, chineseObject, numericObject, symbolicObject));

        // English
        List<MMICUTestObject> matchEnglishObjects = table.getAllObjects(DBMMICUTestObject.content.match("Engl*"));
        Assert.assertTrue(matchEnglishObjects.size() == 1 && matchEnglishObjects.get(0).content.equals(englishObject.content));

        // Chinese
        List<MMICUTestObject> matchChineseObjects = table.getAllObjects(DBMMICUTestObject.content.match("中文"));
        Assert.assertTrue(matchChineseObjects.size() == 1 && matchChineseObjects.get(0).content.equals(chineseObject.content));

        // Numeric
        List<MMICUTestObject> matchNumericObjects = table.getAllObjects(DBMMICUTestObject.content.match("123*"));
        Assert.assertTrue(matchNumericObjects.size() == 1 && matchNumericObjects.get(0).content.equals(numericObject.content));

        // Symbolic
        List<MMICUTestObject> matchSymbolicObjects = table.getAllObjects(DBMMICUTestObject.content.match("def*"));
        Assert.assertTrue(matchSymbolicObjects.size() == 1 && matchSymbolicObjects.get(0).content.equals(symbolicObject.content));
    }

    @Test
    public void testFTS5() {
        database.addTokenizer(BuiltinTokenizer.Verbatim);
        database.createVirtualTable(tableName, DBFTS5TestObject.INSTANCE);
        Table<FTS5TestObject> table = database.getTable(tableName, DBFTS5TestObject.INSTANCE);
        FTS5TestObject englishObject = new FTS5TestObject(1, "This is English test content");
        FTS5TestObject chineseObject = new FTS5TestObject(2, "这是中文测试内容");
        FTS5TestObject numericObject = new FTS5TestObject(3, "123456");
        FTS5TestObject symbolicObject = new FTS5TestObject(4, "abc..def");
        List<FTS5TestObject> allObjects = Arrays.asList(englishObject, chineseObject, numericObject, symbolicObject);
        table.insertObjects(allObjects);

        // External content object
        database.createTable("contentTable", DBTestObject.INSTANCE);
        Table<TestObject> contentTable = database.getTable("contentTable", DBTestObject.INSTANCE);
        List<TestObject> allContentObjects = new ArrayList<>();
        for (FTS5TestObject object : allObjects) {
            TestObject contentObject = TestObject.createObject(object.id, object.content);
            allContentObjects.add(contentObject);
        }
        contentTable.insertObjects(allContentObjects);

        // English
        List<FTS5TestObject> matchEnglishObjects = table.getAllObjects(DBFTS5TestObject.content.match("Engl*"));
        Assert.assertTrue(matchEnglishObjects.size() == 1 && matchEnglishObjects.get(0).content.equals(englishObject.content));

        // Chinese
        List<FTS5TestObject> matchChineseObjects = table.getAllObjects(DBFTS5TestObject.content.match("中文"));
        Assert.assertTrue(matchChineseObjects.size() == 1 && matchChineseObjects.get(0).content.equals(chineseObject.content));

        // Numeric
        List<FTS5TestObject> matchNumericObjects = table.getAllObjects(DBFTS5TestObject.content.match("123*"));
        Assert.assertTrue(matchNumericObjects.size() == 1 && matchNumericObjects.get(0).content.equals(numericObject.content));

        // Symbolic
        List<FTS5TestObject> matchSymbolicObjects = table.getAllObjects(DBFTS5TestObject.content.match("def*"));
        Assert.assertTrue(matchSymbolicObjects.size() == 1 && matchSymbolicObjects.get(0).content.equals(symbolicObject.content));
    }

    @Test
    public void testPinyin() {
        Database.configPinyinDict(new HashMap<String, List<String>>() {{
            put("单", Arrays.asList("shan", "dan", "chan"));
            put("于", Collections.singletonList("yu"));
            put("骑", Collections.singletonList("qi"));
            put("模", Arrays.asList("mo", "mu"));
            put("具", Collections.singletonList("ju"));
            put("车", Collections.singletonList("che"));
        }});
        database.addTokenizer(BuiltinTokenizer.Pinyin);
        database.createVirtualTable(tableName, DBPinyinObject.INSTANCE);

        PinyinObject obj = new PinyinObject();
        obj.content = "单于骑模具单车";
        database.insertObject(obj, DBPinyinObject.allFields(), tableName);

        String[] queries = new String[]{
                "\"shan yu qi mu ju dan che\"",
                "\"chan yu qi mo ju shan che\"",
                "\"dan yu qi mo ju chan che\"",
                "\"dan yu qi mu ju ch\"*",
                "\"dan yu qi mo ju d\"*",
                "\"s y q m j d c\"",
                "\"c y q m j s c\"",
                "\"c y q m j\""
        };

        for (String query : queries) {
            List<PinyinObject> results = database.getAllObjects(DBPinyinObject.allFields(),
                    tableName, DBPinyinObject.content.match(query));
            assert results.size() == 1;
            assert results.get(0).content.equals(obj.content);
        }
    }

    @Test
    public void testTraditionalChinese() {
        Database.configTraditionalChineseDict(new HashMap<String, String>() {{
            put("們", "们");
            put("員", "员");
        }});
        database.addTokenizer(BuiltinTokenizer.Verbatim);
        database.createVirtualTable(tableName, DBTraditionalChineseObject.INSTANCE);
        Table<TraditionalChineseObject> table = database.getTable(tableName, DBTraditionalChineseObject.INSTANCE);

        TraditionalChineseObject obj = new TraditionalChineseObject();
        obj.content = "我們是程序員";
        table.insertObject(obj);

        List<TraditionalChineseObject> matchObject1 = table.getAllObjects(DBTraditionalChineseObject.content.match("我們是程序員"));
        Assert.assertTrue(matchObject1.size() == 1 && matchObject1.get(0).content.equals(obj.content));

        List<TraditionalChineseObject> matchObject2 = table.getAllObjects(DBTraditionalChineseObject.content.match("我们是程序员"));
        Assert.assertTrue(matchObject2.size() == 1 && matchObject2.get(0).content.equals(obj.content));
    }
}

// Created by qiuwenchen on 2023/4/14.
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

package com.tencent.wcdbtest.base;

import com.tencent.wcdb.WCDBField;
import com.tencent.wcdb.WCDBTableCoding;

@WCDBTableCoding
public class TestObject {
    @WCDBField(isPrimary = true, isAutoIncrement = true)
    public int id;
    @WCDBField
    public String content;

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof TestObject) {
            TestObject testObject = (TestObject) obj;
            if(testObject.id != id) {
                return false;
            }
            if(content == null) {
                return testObject.content == null || testObject.content.length() <= 0;
            } else if(testObject.content == null) {
                return content.length() <= 0;
            }
            return testObject.content.equals(content);
        } else {
            return super.equals(obj);
        }
    }

    public static TestObject createObject(int id, String content) {
        TestObject obj = new TestObject();
        obj.id = id;
        obj.content = content;
        return obj;
    }

    public static TestObject createPartialObject(int id) {
        TestObject obj = new TestObject();
        obj.id = id;
        return obj;
    }

    public static TestObject createAutoIncrementObject(String content) {
        TestObject obj = new TestObject();
        obj.id = 0;
        obj.content = content;
        return obj;
    }
}

// Created by chenqiuwen on 2023/5/10.
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

import com.tencent.wcdb.base.Value;

import java.util.Random;

public class RandomTool {

    public static String string() {
        return string(100);
    }

    public static String string(int length) {
        String str="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        Random random = new Random();
        StringBuilder stringBuffer = new StringBuilder(length);
        for(int i = 0; i < length; i++){
            int number=random.nextInt(62);
            stringBuffer.append(str.charAt(number));
        }
        return stringBuffer.toString();
    }

    public static byte[] bytes() {
        return bytes(100);
    }

    public static byte[] bytes(int length) {
        Random random = new Random();
        byte[] bytes = new byte[length];
        random.nextBytes(bytes);
        return bytes;
    }

    public static TestObject[] autoIncrementTestCaseObjects(int count) {
        TestObject[] objects = new TestObject[count];
        for(int i = 0; i < count; i++) {
            objects[i] = TestObject.createAutoIncrementObject(string());
        }
        return objects;
    }

    public static TestObject testObjectWithId(int id) {
        return TestObject.createObject(id, string());
    }

    public static Value[] testRowWithId(int id) {
        return new Value[]{ new Value(id), new Value(string())};
    }

}

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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

public class RandomTool {

    public static String string() {
        return string(100);
    }

    public static String string(int length) {
        String str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        Random random = new Random();
        StringBuilder stringBuffer = new StringBuilder(length);
        for (int i = 0; i < length; i++) {
            int number = random.nextInt(62);
            stringBuffer.append(str.charAt(number));
        }
        return stringBuffer.toString();
    }

    private static final ArrayList<String> g_englishWordArray = createEnglishWordArray();

    private static ArrayList<String> createEnglishWordArray() {
        Map<String, Integer> g_wordFrequency = new HashMap<>();
        g_wordFrequency.put("a", 408);
        g_wordFrequency.put("he", 121);
        g_wordFrequency.put("but", 86);
        g_wordFrequency.put("my", 58);
        g_wordFrequency.put("if", 51);
        g_wordFrequency.put("just", 44);
        g_wordFrequency.put("people", 33);
        g_wordFrequency.put("think", 28);
        g_wordFrequency.put("because", 25);
        g_wordFrequency.put("two", 21);
        g_wordFrequency.put("very", 19);
        g_wordFrequency.put("should", 17);
        g_wordFrequency.put("work", 15);
        g_wordFrequency.put("day", 14);
        g_wordFrequency.put("oh", 13);
        g_wordFrequency.put("use", 11);
        g_wordFrequency.put("lot", 10);
        g_wordFrequency.put("mr", 10);
        g_wordFrequency.put("part", 9);
        g_wordFrequency.put("without", 8);
        ArrayList<String> wordArray = new ArrayList<>(1000);
        for (Map.Entry<String, Integer> entry : g_wordFrequency.entrySet()) {
            String word = entry.getKey();
            int frequency = entry.getValue();
            for (int i = 0; i < frequency; i++) {
                wordArray.add(word);
            }
        }
        return wordArray;
    }

    public static String englishString() {
        return englishString(100);
    }

    public static String englishString(int length) {
        StringBuilder ret = new StringBuilder();
        for (int i = 0; i < length; i++) {
            ret.append(g_englishWordArray.get((int) (Math.random() * 1000)));
            ret.append(" ");
        }
        return ret.toString();
    }

    public static boolean useEnglishWord = false;

    public static byte[] bytes() {
        return bytes(100);
    }

    public static byte[] bytes(int length) {
        Random random = new Random();
        byte[] bytes = new byte[length];
        random.nextBytes(bytes);
        return bytes;
    }

    public static List<TestObject> autoIncrementTestCaseObjects(int count) {
        ArrayList<TestObject> objects = new ArrayList<>();
        for (int i = 0; i < count; i++) {
            objects.add(TestObject.createAutoIncrementObject(useEnglishWord ? englishString() : string()));
        }
        return objects;
    }

    public static List<TestObject> testCaseObjects(int startIdentifier, int count) {
        ArrayList<TestObject> objects = new ArrayList<>();
        for (int i = 0; i < count; i++) {
            objects.add(TestObject.createObject(i + startIdentifier, useEnglishWord ? englishString() : string()));
        }
        return objects;
    }

    public static TestObject testObjectWithId(int id) {
        return TestObject.createObject(id, useEnglishWord ? englishString() : string());
    }

    public static Value[] testRowWithId(int id) {
        return new Value[]{new Value(id), new Value(useEnglishWord ? englishString() : string())};
    }

}

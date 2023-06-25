// Created by chenqiuwen on 2023/6/24.
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

package com.tencent.wcdbtest.base

import com.tencent.wcdb.base.Value
import java.util.*

object RandomTool {
    @JvmOverloads
    fun string(length: Int = 100): String {
        val str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        val random = Random()
        val stringBuffer = StringBuilder(length)
        for (i in 0 until length) {
            val number = random.nextInt(62)
            stringBuffer.append(str[number])
        }
        return stringBuffer.toString()
    }

    @JvmOverloads
    fun bytes(length: Int = 100): ByteArray {
        val random = Random()
        val bytes = ByteArray(length)
        random.nextBytes(bytes)
        return bytes
    }

    fun autoIncrementTestCaseObjects(count: Int): Array<TestObject?> {
        val objects = arrayOfNulls<TestObject>(count)
        for (i in 0 until count) {
            objects[i] = TestObject.createAutoIncrementObject(string())
        }
        return objects
    }

    fun testObjectWithId(id: Int): TestObject {
        return TestObject.createObject(id, string())
    }

    fun testRowWithId(id: Int): Array<Value> {
        return arrayOf(Value(id.toLong()), Value(string()))
    }
}
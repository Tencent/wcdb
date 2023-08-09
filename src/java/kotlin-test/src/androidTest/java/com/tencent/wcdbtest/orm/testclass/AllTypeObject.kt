// Created by qiuwenchen on 2023/6/25.
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

package com.tencent.wcdbtest.orm.testclass

import com.tencent.wcdb.WCDBField
import com.tencent.wcdb.WCDBTableCoding
import com.tencent.wcdbtest.base.RandomTool
import java.util.*

@WCDBTableCoding
class AllTypeObject {
    @WCDBField
    var type: String = ""

    //Integer
    @WCDBField
    var aBoolean = false
    @WCDBField
    var aBoolean2: Boolean? = null
    @WCDBField
    var aChar = 0.toChar()
    @WCDBField
    var character: Char? = null
    @WCDBField
    var aByte: Byte = 0
    @WCDBField
    var aByte2: Byte? = null
    @WCDBField
    var aShort: Short = 0
    @WCDBField
    var aShort2: Short? = null
    @WCDBField
    var anInt = 0
    @WCDBField
    var integer: Int? = null
    @WCDBField
    var aLong: Long = 0
    @WCDBField
    var aLong2: Long? = null

    // Float
    @WCDBField
    var aFloat = 0f
    @WCDBField
    var aFloat2: Float? = null
    @WCDBField
    var aDouble = 0.0
    @WCDBField
    var aDouble2: Double? = null

    // String
    @WCDBField
    var string: String? = null

    // BLOB
    @WCDBField
    var bytes: ByteArray = byteArrayOf()
    @WCDBField
    var bytes2: ByteArray? = null

    override fun equals(other: Any?): Boolean {
        if (other == null) {
            return false
        }
        if (other !is AllTypeObject) {
            return super.equals(other)
        }
        return aBoolean == other.aBoolean &&
                getIntegerValue(aBoolean2) == getIntegerValue(other.aBoolean2) &&
                aChar == other.aChar &&
                getIntegerValue(character ) == getIntegerValue(other.character) &&
                aShort == other.aShort &&
                getIntegerValue(aShort2) == getIntegerValue(other.aShort2) &&
                anInt == other.anInt &&
                getIntegerValue(integer) == getIntegerValue(other.integer) &&
                aLong == other.aLong &&
                getIntegerValue(aLong2) == getIntegerValue(other.aLong2) &&
                aFloat == other.aFloat &&
                getDoubleValue(aFloat2) == getDoubleValue(other.aFloat2) &&
                aDouble == other.aDouble &&
                getDoubleValue(aDouble2) == getDoubleValue(other.aDouble2) &&
                stringCompare(string, other.string) &&
                getNotNullBuffer(bytes).contentEquals(getNotNullBuffer(other.bytes)) &&
                getNotNullBuffer(bytes2).contentEquals(getNotNullBuffer(other.bytes2))
    }

    fun getNotNullBuffer(byteArray: ByteArray?): ByteArray {
        if(byteArray == null) {
            return byteArrayOf()
        }
        return byteArray
    }

    private fun getIntegerValue(`object`: Any?): Long {
        if (`object` == null) {
            return 0
        }
        return if (`object` is Boolean) {
            if (`object`) 1 else 0
        } else if (`object` is Char) {
            `object`.toLong()
        } else if (`object` is Byte) {
            `object`.toLong()
        } else if (`object` is Short) {
            `object`.toLong()
        } else if (`object` is Int) {
            `object`.toLong()
        } else {
            `object` as Long
        }
    }

    private fun getDoubleValue(`object`: Any?): Double {
        if (`object` == null) {
            return 0.0
        }
        return if (`object` is Float) {
            `object`.toDouble()
        } else {
            `object` as Double
        }
    }

    private fun stringCompare(obj1: String?, obj2: String?): Boolean {
        if (obj1 === obj2) {
            return true
        }
        if (obj1 == null && obj2!!.isEmpty()) {
            return true
        }
        return if (obj2 == null && obj1!!.isEmpty()) {
            true
        } else obj1 == obj2
    }

    companion object {
        fun maxObject(): AllTypeObject {
            val max = AllTypeObject()
            max.type = "max"
            max.aBoolean = true
            max.aBoolean2 = true
            max.aByte = Byte.MAX_VALUE
            max.aByte2 = Byte.MAX_VALUE
            max.aChar = Character.MAX_VALUE
            max.character = Character.MAX_VALUE
            max.aShort = Short.MAX_VALUE
            max.aShort2 = Short.MAX_VALUE
            max.anInt = Int.MAX_VALUE
            max.integer = Int.MAX_VALUE
            max.aLong = Long.MAX_VALUE
            max.aLong2 = Long.MAX_VALUE
            max.aFloat = Float.MAX_VALUE
            max.aFloat2 = Float.MAX_VALUE
            max.aDouble = Double.MAX_VALUE
            max.aDouble2 = Double.MAX_VALUE
            max.string = RandomTool.string()
            max.bytes = RandomTool.bytes()
            max.bytes2 = RandomTool.bytes()
            return max
        }

        fun minObject(): AllTypeObject {
            val min = AllTypeObject()
            min.type = "min"
            min.aBoolean = false
            min.aBoolean2 = false
            min.aByte = Byte.MIN_VALUE
            min.aByte2 = Byte.MIN_VALUE
            min.aChar = Character.MIN_VALUE
            min.character = Character.MIN_VALUE
            min.aShort = Short.MIN_VALUE
            min.aShort2 = Short.MIN_VALUE
            min.anInt = Int.MIN_VALUE
            min.integer = Int.MIN_VALUE
            min.aLong = Long.MIN_VALUE
            min.aLong2 = Long.MIN_VALUE
            min.aFloat = Float.MIN_VALUE
            min.aFloat2 = Float.MIN_VALUE
            min.aDouble = Double.MIN_VALUE
            min.aDouble2 = Double.MIN_VALUE
            min.string = RandomTool.string()
            min.bytes = RandomTool.bytes()
            min.bytes2 = RandomTool.bytes()
            return min
        }

        fun randomObject(): AllTypeObject {
            val random = Random()
            val `object` = AllTypeObject()
            `object`.type = "random"
            `object`.aBoolean = random.nextBoolean()
            `object`.aBoolean2 = random.nextBoolean()
            `object`.aByte = random.nextInt().toByte()
            `object`.aByte2 = random.nextInt().toByte()
            `object`.aChar = random.nextInt().toChar()
            `object`.character = random.nextInt().toChar()
            `object`.aShort = random.nextInt().toShort()
            `object`.aShort2 = random.nextInt().toShort()
            `object`.anInt = random.nextInt()
            `object`.integer = random.nextInt()
            `object`.aLong = random.nextLong()
            `object`.aLong2 = random.nextLong()
            `object`.aFloat = random.nextFloat()
            `object`.aFloat2 = random.nextFloat()
            `object`.aDouble = random.nextDouble()
            `object`.aDouble2 = random.nextDouble()
            `object`.string = RandomTool.string()
            `object`.bytes = RandomTool.bytes()
            `object`.bytes2 = RandomTool.bytes()
            return `object`
        }

        fun emptyObject(): AllTypeObject {
            val empty = AllTypeObject()
            empty.type = "empty"
            empty.bytes = byteArrayOf()
            return empty
        }
    }
}
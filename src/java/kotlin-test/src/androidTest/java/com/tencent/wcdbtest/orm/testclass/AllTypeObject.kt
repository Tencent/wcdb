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
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as AllTypeObject

        if (aBoolean != other.aBoolean) return false
        if (aBoolean2 != other.aBoolean2) return false
        if (aByte != other.aByte) return false
        if (aByte2 != other.aByte2) return false
        if (aShort != other.aShort) return false
        if (aShort2 != other.aShort2) return false
        if (anInt != other.anInt) return false
        if (integer != other.integer) return false
        if (aLong != other.aLong) return false
        if (aLong2 != other.aLong2) return false
        if (aFloat != other.aFloat) return false
        if (aFloat2 != other.aFloat2) return false
        if (aDouble != other.aDouble) return false
        if (aDouble2 != other.aDouble2) return false
        if (string != other.string) return false
        if (!bytes.contentEquals(other.bytes)) return false
        if (bytes2 != null) {
            if (other.bytes2 == null) return false
            if (!bytes2.contentEquals(other.bytes2)) return false
        } else if (other.bytes2 != null) return false

        return true
    }


    companion object {
        fun maxObject(): AllTypeObject {
            val max = AllTypeObject()
            max.type = "max"
            max.aBoolean = true
            max.aBoolean2 = true
            max.aByte = Byte.MAX_VALUE
            max.aByte2 = Byte.MAX_VALUE
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
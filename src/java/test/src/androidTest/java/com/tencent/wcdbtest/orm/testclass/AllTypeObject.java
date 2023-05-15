// Created by qiuwenchen on 2023/5/12.
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

package com.tencent.wcdbtest.orm.testclass;
import androidx.annotation.Nullable;

import com.tencent.wcdb.*;
import com.tencent.wcdbtest.base.RandomTool;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Random;

@WCDBTableCoding
public class AllTypeObject {
    @WCDBField
    public
    String type;

    //Integer
    @WCDBField
    boolean aBoolean;
    @WCDBField
    Boolean aBoolean2;
    @WCDBField
    char aChar;
    @WCDBField
    Character character;
    @WCDBField
    byte aByte;
    @WCDBField
    Byte aByte2;
    @WCDBField
    short aShort;
    @WCDBField
    Short aShort2;
    @WCDBField
    int anInt;
    @WCDBField
    Integer integer;
    @WCDBField
    long aLong;
    @WCDBField
    Long aLong2;

    // Float
    @WCDBField
    float aFloat;
    @WCDBField
    Float aFloat2;
    @WCDBField
    double aDouble;
    @WCDBField
    Double aDouble2;

    // String
    @WCDBField
    String string;

    // BLOB
    @WCDBField
    byte[] bytes;

    static public AllTypeObject maxObject() {
        AllTypeObject max = new AllTypeObject();
        max.type = "max";
        max.aBoolean = true;
        max.aBoolean2 = true;
        max.aByte = Byte.MAX_VALUE;
        max.aByte2 = Byte.MAX_VALUE;
        max.aChar = Character.MAX_VALUE;
        max.character = Character.MAX_VALUE;
        max.aShort = Short.MAX_VALUE;
        max.aShort2 = Short.MAX_VALUE;
        max.anInt = Integer.MAX_VALUE;
        max.integer = Integer.MAX_VALUE;
        max.aLong = Long.MAX_VALUE;
        max.aLong2 = Long.MAX_VALUE;
        max.aFloat = Float.MAX_VALUE;
        max.aFloat2 = Float.MAX_VALUE;
        max.aDouble = Double.MAX_VALUE;
        max.aDouble2 = Double.MAX_VALUE;
        max.string = RandomTool.string();
        max.bytes = RandomTool.bytes();
        return max;
    }

    static public AllTypeObject minObject() {
        AllTypeObject min = new AllTypeObject();
        min.type = "min";
        min.aBoolean = false;
        min.aBoolean2 = false;
        min.aByte = Byte.MIN_VALUE;
        min.aByte2 = Byte.MIN_VALUE;
        min.aChar = Character.MIN_VALUE;
        min.character = Character.MIN_VALUE;
        min.aShort = Short.MIN_VALUE;
        min.aShort2 = Short.MIN_VALUE;
        min.anInt = Integer.MIN_VALUE;
        min.integer = Integer.MIN_VALUE;
        min.aLong = Long.MIN_VALUE;
        min.aLong2 = Long.MIN_VALUE;
        min.aFloat = Float.MIN_VALUE;
        min.aFloat2 = Float.MIN_VALUE;
        min.aDouble = Double.MIN_VALUE;
        min.aDouble2 = Double.MIN_VALUE;
        min.string = RandomTool.string();
        min.bytes = RandomTool.bytes();
        return min;
    }

    static public AllTypeObject randomObject() {
        Random random = new Random();
        AllTypeObject object = new AllTypeObject();
        object.type = "random";
        object.aBoolean = random.nextBoolean();
        object.aBoolean2 = random.nextBoolean();
        object.aByte = (byte) random.nextInt();
        object.aByte2 = (byte) random.nextInt();
        object.aChar = (char) random.nextInt();
        object.character = (char) random.nextInt();
        object.aShort = (short) random.nextInt();
        object.aShort2 = (short) random.nextInt();
        object.anInt = random.nextInt();
        object.integer = random.nextInt();
        object.aLong = random.nextLong();
        object.aLong2 = random.nextLong();
        object.aFloat = random.nextFloat();
        object.aFloat2 = random.nextFloat();
        object.aDouble = random.nextDouble();
        object.aDouble2 = random.nextDouble();
        object.string = RandomTool.string();
        object.bytes = RandomTool.bytes();
        return object;
    }

    static public AllTypeObject emptyObject() {
        AllTypeObject empty = new AllTypeObject();
        empty.type = "empty";
        return empty;
    }

    @Override
    public boolean equals(@Nullable Object obj) {
        if(!(obj instanceof AllTypeObject)) {
            return super.equals(obj);
        }
        AllTypeObject other = (AllTypeObject) obj;
        return aBoolean == other.aBoolean &&
                getIntegerValue(aBoolean2) == getIntegerValue(other.aBoolean2) &&
                aChar == other.aChar &&
                getIntegerValue(character) == getIntegerValue(other.character)  &&
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
                Arrays.equals(bytes, other.bytes);
    }

    long getIntegerValue(Object object) {
        if(object == null) {
            return 0;
        }
        if (object instanceof Boolean) {
            return (Boolean)object ? 1: 0;
        } else if (object instanceof Character) {
            return (Character)object;
        } else if (object instanceof Byte) {
            return (Byte) object;
        } else if (object instanceof Short) {
            return (Short) object;
        } else if (object instanceof Integer) {
            return (Integer) object;
        } else {
            return (Long) object;
        }
    }

    double getDoubleValue(Object object) {
        if(object == null) {
            return 0;
        }
        if(object instanceof Float) {
            return (Float) object;
        } else {
            return (Double) object;
        }
    }

    boolean stringCompare(String obj1, String obj2) {
        if(obj1 == obj2) {
            return true;
        }
        if(obj1 == null && obj2.length() == 0) {
            return true;
        }
        if(obj2 == null && obj1.length() == 0) {
            return true;
        }
        return obj1.equals(obj2);
    }
}

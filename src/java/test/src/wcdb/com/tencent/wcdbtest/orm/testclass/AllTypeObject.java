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

import com.tencent.wcdb.*;

import java.util.Arrays;

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

    @Override
    public boolean equals(Object obj) {
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
                blobCompare(bytes, other.bytes);
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

    boolean blobCompare(byte[] obj1, byte[] obj2) {
        if(obj1 == obj2) {
            return true;
        }
        if(obj1 == null && obj2.length == 0) {
            return true;
        }
        if(obj2 == null && obj1.length == 0) {
            return true;
        }
        return Arrays.equals(obj1, obj2);
    }
}

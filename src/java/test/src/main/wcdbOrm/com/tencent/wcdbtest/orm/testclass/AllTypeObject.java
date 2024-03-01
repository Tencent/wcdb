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
import java.util.Objects;

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
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        AllTypeObject that = (AllTypeObject) o;
        return aBoolean == that.aBoolean &&
                aByte == that.aByte &&
                aShort == that.aShort &&
                anInt == that.anInt &&
                aLong == that.aLong &&
                Float.compare(that.aFloat, aFloat) == 0 &&
                Double.compare(that.aDouble, aDouble) == 0 &&
                Objects.equals(aBoolean2, that.aBoolean2) &&
                Objects.equals(aByte2, that.aByte2) &&
                Objects.equals(aShort2, that.aShort2) &&
                Objects.equals(integer, that.integer) &&
                Objects.equals(aLong2, that.aLong2) &&
                Objects.equals(aFloat2, that.aFloat2) &&
                Objects.equals(aDouble2, that.aDouble2) &&
                Objects.equals(string, that.string) &&
                Arrays.equals(bytes, that.bytes);
    }
}

// Created by qiuwenchen on 2023/3/30.
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

package com.tencent.wcdb.winq;

public class LiteralValue extends Identifier implements ExpressionConvertible {
    @Override
    protected CPPType getCppType() {
        return CPPType.LiteralValue;
    }

    @Override
    public Identifier asIdentifier() {
        return this;
    }

    public LiteralValue(long value) {
        cppObj = createCppObj(CPPType.Int.ordinal(), value, 0, null);
    }

    public LiteralValue(double value) {
        cppObj = createCppObj(CPPType.Double.ordinal(), 0, value, null);
    }

    public LiteralValue(boolean value) {
        cppObj = createCppObj(CPPType.Bool.ordinal(), value ? 1 : 0, 0, null);
    }

    public LiteralValue(String value) {
        if(value == null) {
            cppObj = createCppObj(CPPType.Null.ordinal(), 0, 0, null);
        } else {
            cppObj = createCppObj(CPPType.String.ordinal(), 0, 0, value);
        }
    }

    private native long createCppObj(int type, long intValue, double doubleValue, String stringValue);

    private LiteralValue(){
    }

    public static LiteralValue currentTime() {
        LiteralValue ret = new LiteralValue();
        ret.cppObj = createCurrentTime();
        return ret;
    }

    private native static long createCurrentTime();

    public static LiteralValue currentDate() {
        LiteralValue ret = new LiteralValue();
        ret.cppObj = createCurrentDate();
        return ret;
    }

    private native static long createCurrentDate();

    public static LiteralValue currentTimeStamp() {
        LiteralValue ret = new LiteralValue();
        ret.cppObj = createCurrentTimeStamp();
        return ret;
    }

    private native static long createCurrentTimeStamp();
}

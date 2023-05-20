// Created by qiuwenchen on 2023/4/26.
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

package com.tencent.wcdb.base;

import com.tencent.wcdb.winq.ColumnType;

import java.util.Arrays;

public class Value {
    public Value() {
        type = ColumnType.Null;
    }

    public Value(long value) {
        intValue = value;
        type = ColumnType.Integer;
    }

    public Value(double value) {
        doubleValue = value;
        type = ColumnType.Float;
    }

    public Value(String value) {
        if(value == null) {
            type = ColumnType.Null;
        }else{
            textValue = value;
            type = ColumnType.Text;
        }
    }

    public Value(byte[] value) {
        if(value == null) {
            type = ColumnType.Null;
        }else{
            blobValue = value;
            type = ColumnType.BLOB;
        }
    }

    public ColumnType getType(){
        return type;
    }

    public boolean getBool() {
        return getInteger() > 0;
    }

    public long getInteger(){
        switch (type) {
            case Integer:
                return intValue;
            case Float:
                return (long) doubleValue;
            case Text:
                return Long.parseLong(textValue);
            default:
                return 0;
        }
    }

    public double getDouble() {
        switch (type) {
            case Float:
                return doubleValue;
            case Integer:
                return intValue;
            case Text:
                return Double.parseDouble(textValue);
            default:
                return 0;
        }
    }

    public String getText() {
        switch (type) {
            case Text:
                return textValue;
            case Integer:
                return String.valueOf(intValue);
            case Float:
                return String.valueOf(doubleValue);
            default:
                return null;
        }
    }

    public byte[] getBLOB() {
        switch (type) {
            case Text:
                return textValue.getBytes();
            case BLOB:
                return blobValue;
            default:
                return null;
        }
    }

    @Override
    public boolean equals(Object obj) {
        if(obj instanceof Value) {
            Value value = (Value) obj;
            switch (type) {
                case Null:
                    return value.getInteger() == 0;
                case Integer:
                    return intValue == value.getInteger();
                case Float:
                    return doubleValue == value.getDouble();
                case Text:
                    return textValue.equals(value.getText());
                case BLOB:
                    return Arrays.equals(blobValue, value.getBLOB());
            }
        } else if (obj == null) {
            switch (type) {
                case Null:
                    return true;
                case Integer:
                    return intValue == 0;
                case Float:
                    return doubleValue == 0;
                case Text:
                    return textValue == null || textValue.length() == 0;
                case BLOB:
                    return blobValue == null || blobValue.length == 0;
            }
        }
        return super.equals(obj);
    }

    @Override
    public int hashCode() {
        switch (type) {
            case Null:
                return 0;
            case Integer:
                return Long.valueOf(intValue).hashCode();
            case Float:
                return Double.valueOf(doubleValue).hashCode();
            case Text:
                return textValue.hashCode();
            case BLOB:
                return Arrays.hashCode(blobValue);
        }
        return 0;
    }

    ColumnType type;
    long intValue;
    double doubleValue;
    String textValue;
    byte[] blobValue;
}

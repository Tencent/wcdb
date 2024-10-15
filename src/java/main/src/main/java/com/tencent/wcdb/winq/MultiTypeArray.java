// Created by qiuwenchen on 2023/4/14.
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

import com.tencent.wcdb.base.CppObject;
import com.tencent.wcdb.base.Value;

import java.util.Arrays;

class ObjectType {
    final static int Null = 0;
    final static int Bool = 1;
    final static int Char = 2;
    final static int Byte = 3;
    final static int Short = 4;
    final static int Int = 5;
    final static int Long = 6;
    final static int Float = 7;
    final static int Double = 8;
    final static int String = 9;
    final static int Identifier = 10;

    final static int Value = 11;
    final static int Unknown = 12;
}

class MultiTypeArray {

    int[] types;
    long[] longValues;
    double[] doubleValues;
    String[] stringValues;

    MultiTypeArray(Object[] values) {
        int valueCount = values.length;
        types = new int[valueCount];
        longValues = new long[valueCount];
        doubleValues = new double[valueCount];
        stringValues = new String[valueCount];
        int longIndex = 0;
        int doubleIndex = 0;
        int stringIndex = 0;

        for (int i = 0; i < valueCount; i++) {
            Object obj = values[i];
            int valueType = getObjectType(obj);
            switch (valueType) {
                case ObjectType.Null:
                    types[i] = CPPType.Null;
                    longIndex++;
                    break;
                case ObjectType.Bool:
                    types[i] = CPPType.Bool;
                    longValues[longIndex] = (Boolean) obj ? 1 : 0;
                    longIndex++;
                    break;
                case ObjectType.Byte:
                    types[i] = CPPType.Int;
                    longValues[longIndex] = (Byte) obj;
                    longIndex++;
                    break;
                case ObjectType.Char:
                    types[i] = CPPType.Int;
                    longValues[longIndex] = (Character) obj;
                    longIndex++;
                    break;
                case ObjectType.Short:
                    types[i] = CPPType.Int;
                    longValues[longIndex] = (Short) obj;
                    longIndex++;
                    break;
                case ObjectType.Int:
                    types[i] = CPPType.Int;
                    longValues[longIndex] = (Integer) obj;
                    longIndex++;
                    break;
                case ObjectType.Long:
                    types[i] = CPPType.Int;
                    longValues[longIndex] = (Long) obj;
                    longIndex++;
                    break;
                case ObjectType.Float:
                    types[i] = CPPType.Double;
                    doubleValues[doubleIndex] = (Float) obj;
                    doubleIndex++;
                    break;
                case ObjectType.Double:
                    types[i] = CPPType.Double;
                    doubleValues[doubleIndex] = (Double) obj;
                    doubleIndex++;
                    break;
                case ObjectType.String:
                    types[i] = CPPType.String;
                    stringValues[stringIndex] = (String) obj;
                    stringIndex++;
                    break;
                case ObjectType.Identifier:
                    Identifier identifier = (Identifier) obj;
                    types[i] = Identifier.getCppType(identifier);
                    longValues[longIndex] = CppObject.get(identifier);
                    longIndex++;
                    break;
                case ObjectType.Value:
                    Value valueObj = (Value) obj;
                    switch (valueObj.getType()) {
                        case Null:
                            types[i] = CPPType.Null;
                            longIndex++;
                            break;
                        case Integer:
                            types[i] = CPPType.Int;
                            longValues[longIndex] = valueObj.getLong();
                            longIndex++;
                            break;
                        case Float:
                            types[i] = CPPType.Double;
                            doubleValues[doubleIndex] = valueObj.getDouble();
                            doubleIndex++;
                            break;
                        case Text:
                            types[i] = CPPType.String;
                            stringValues[stringIndex] = valueObj.getText();
                            stringIndex++;
                            break;
                    }
                    break;
            }
        }
        if (stringValues.length * 0.75 > stringIndex) {
            if (stringIndex == 0) {
                stringValues = null;
            } else {
                stringValues = Arrays.copyOf(stringValues, stringIndex);
            }
        }
    }

    protected static int getObjectType(Object obj) {
        if (obj == null) {
            return ObjectType.Null;
        } else if (obj instanceof Identifier) {
            return ObjectType.Identifier;
        }
        Class cls = obj.getClass();
        if (cls == String.class) {
            return ObjectType.String;
        } else if (cls == Integer.class) {
            return ObjectType.Int;
        } else if (cls == Float.class) {
            return ObjectType.Float;
        } else if (cls == Double.class) {
            return ObjectType.Double;
        } else if (cls == Boolean.class) {
            return ObjectType.Bool;
        } else if (cls == Short.class) {
            return ObjectType.Short;
        } else if (cls == Long.class) {
            return ObjectType.Long;
        } else if (cls == Character.class) {
            return ObjectType.Char;
        } else if (cls == Byte.class) {
            return ObjectType.Byte;
        } else if (cls == Value.class) {
            return ObjectType.Value;
        }
        return ObjectType.Unknown;
    }
}

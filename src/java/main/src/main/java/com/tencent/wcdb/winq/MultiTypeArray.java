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

import java.util.Arrays;

enum ObjectType {
    Null,
    Bool,
    Char,
    Short,
    Int,
    Long,
    Float,
    Double,
    String,
    Identifier,
    Unknown
}

class MultiTypeArray {

    int[] types;
    long[] longValues;
    double[] doubleValues;
    String[] stringValues;

    MultiTypeArray(Object [] values) {
        int valueCount = values.length;
        types = new int[valueCount];
        longValues = new long[valueCount];
        doubleValues = new double[valueCount];
        stringValues = new String[valueCount];
        int longIndex = 0;
        int doubleIndex = 0;
        int stringIndex = 0;

        for(int i = 0; i < valueCount; i++) {
            Object obj = values[i];
            ObjectType valueType = getObjectType(obj);
            switch (valueType) {
                case Null:
                    types[i] = CPPType.Null.ordinal();
                    longIndex++;
                    break;
                case Bool:
                    types[i] = CPPType.Bool.ordinal();
                    longValues[longIndex] = (Boolean)obj ? 1 : 0;
                    longIndex++;
                    break;
                case Char:
                    types[i] = CPPType.Int.ordinal();
                    longValues[longIndex] = (Character)obj;
                    longIndex++;
                    break;
                case Short:
                    types[i] = CPPType.Int.ordinal();
                    longValues[longIndex] = (Short)obj;
                    longIndex++;
                    break;
                case Int:
                    types[i] = CPPType.Int.ordinal();
                    longValues[longIndex] = (Integer)obj;
                    longIndex++;
                    break;
                case Long:
                    types[i] = CPPType.Int.ordinal();
                    longValues[longIndex] = (Long) obj;
                    longIndex++;
                    break;
                case Float:
                    types[i] = CPPType.Double.ordinal();
                    doubleValues[doubleIndex] = (Float) obj;
                    doubleIndex++;
                    break;
                case Double:
                    types[i] = CPPType.Double.ordinal();
                    doubleValues[doubleIndex] = (Double) obj;
                    doubleIndex++;
                    break;
                case String:
                    types[i] = CPPType.String.ordinal();
                    stringValues[stringIndex] = (String) obj;
                    stringIndex++;
                    break;
                case Identifier:
                    Identifier identifier = (Identifier)obj;
                    types[i] = identifier.getCppType().ordinal();
                    longValues[longIndex] = identifier.getCppObj();
                    longIndex++;
                    break;
            }
        }
        if(stringValues.length * 0.75 > stringIndex) {
            if(stringIndex == 0) {
                stringValues = null;
            } else {
                stringValues = Arrays.copyOf(stringValues, stringIndex);
            }
        }
    }

    protected static ObjectType getObjectType(Object obj) {
        if (obj == null) {
            return ObjectType.Null;
        } else if (obj instanceof Identifier) {
            return ObjectType.Identifier;
        }
        Class cls = obj.getClass();
        if(cls == String.class) {
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
        }
        return ObjectType.Unknown;
    }
}

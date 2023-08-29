// Created by qiuwenchen on 2023/6/9.
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

public class Upsert extends Identifier{
    @Override
    protected CPPType getType() {
        return CPPType.UpsertClause;
    }

    public Upsert() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    public Upsert onConflict() {
        return this;
    }

    public Upsert indexedBy(String columnName) {
        configIndexedColumn(cppObj, CPPType.String.ordinal(), null, new String[]{columnName});
        return this;
    }

    public Upsert indexedBy(IndexedColumnConvertible indexedColumn) {
        configIndexedColumn(cppObj, Identifier.getCppType(indexedColumn), new long[]{CppObject.get(indexedColumn)}, null);
        return this;
    }

    public Upsert indexedBy(String[] columnNames) {
        configIndexedColumn(cppObj, CPPType.String.ordinal(), null, columnNames);
        return this;
    }

    public Upsert indexedBy(IndexedColumnConvertible[] indexedColumns) {
        if (indexedColumns == null || indexedColumns.length == 0) {
            return this;
        }

        long[] cppIndexedColumns = new long[indexedColumns.length];
        for(int i = 0; i < indexedColumns.length; i++) {
            cppIndexedColumns[i] = CppObject.get(indexedColumns[i]);
        }

        configIndexedColumn(cppObj, Identifier.getCppType(indexedColumns[0]), cppIndexedColumns, null);
        return this;
    }

    private static native void configIndexedColumn(long self, int type, long[] columns, String[] columnNames);

    public Upsert where(Expression condition) {
        configWhere(cppObj, CppObject.get(condition));
        return this;
    }

    private static native void configWhere(long self, long condition);

    public Upsert doNoThing() {
        configDoNothing(cppObj);
        return this;
    }

    private static native void configDoNothing(long self);

    public Upsert doUpdate() {
        configDoUpdate(cppObj);
        return this;
    }

    private static native void configDoUpdate(long self);

    public Upsert setColumn(String columnName) {
        configSetColumns(cppObj, CPPType.String.ordinal(), null, new String[]{columnName});
        return this;
    }

    public Upsert setColumn(Column column) {
        configSetColumns(cppObj, Identifier.getCppType(column), new long[]{CppObject.get(column)}, null);
        return this;
    }

    public Upsert setColumns(String[] columnNames) {
        configSetColumns(cppObj, CPPType.String.ordinal(), null, columnNames);
        return this;
    }

    public Upsert setColumns(Column[] columns) {
        if (columns == null || columns.length == 0) {
            return this;
        }

        long[] cppColumns = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppColumns[i] = CppObject.get(columns[i]);
        }

        configSetColumns(cppObj, Identifier.getCppType(columns[0]), cppColumns, null);
        return this;
    }

    private static native void configSetColumns(long self, int type, long[] columns, String[] columnNames);

    public Upsert to(boolean value) {
        configToValue(cppObj, CPPType.Bool.ordinal(), value ? 1 : 0, 0, null);
        return this;
    }

    public Upsert to(byte value) {
        configToValue(cppObj, CPPType.Int.ordinal(), value, 0, null);
        return this;
    }

    public Upsert to(char value) {
        configToValue(cppObj, CPPType.Int.ordinal(), value, 0, null);
        return this;
    }

    public Upsert to(short value) {
        configToValue(cppObj, CPPType.Int.ordinal(), value, 0, null);
        return this;
    }

    public Upsert to(int value) {
        configToValue(cppObj, CPPType.Int.ordinal(), value, 0, null);
        return this;
    }

    public Upsert to(long value) {
        configToValue(cppObj, CPPType.Int.ordinal(), value, 0, null);
        return this;
    }

    public Upsert to(float value) {
        configToValue(cppObj, CPPType.Double.ordinal(), 0, value, null);
        return this;
    }

    public Upsert to(double value) {
        configToValue(cppObj, CPPType.Double.ordinal(), 0, value, null);
        return this;
    }

    public Upsert to(String value) {
        if(value != null) {
            configToValue(cppObj, CPPType.String.ordinal(), 0, 0, value);
        }else{
            configToValue(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }
        return this;
    }

    public Upsert to(ExpressionConvertible value) {
        configToValue(cppObj, Identifier.getCppType(value), CppObject.get(value), 0, null);
        return this;
    }

    private static native void configToValue(long self, int type, long intValue, double doubleValue, String stringValue);
}

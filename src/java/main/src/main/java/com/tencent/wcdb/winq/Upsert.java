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

import com.tencent.wcdb.chaincall.Update;

public class Upsert extends Identifier{
    @Override
    protected CPPType getCppType() {
        return CPPType.UpsertClause;
    }

    public Upsert() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public Upsert onConflict() {
        return this;
    }

    public Upsert indexedBy(String columnName) {
        configIndexedColumn(cppObj, CPPType.String.ordinal(), null, new String[]{columnName});
        return this;
    }

    public Upsert indexedBy(IndexedColumnConvertible indexedColumn) {
        assert indexedColumn != null;
        if(indexedColumn == null) {
            return this;
        }
        configIndexedColumn(cppObj, indexedColumn.asIdentifier().getCppType().ordinal(), new long[]{indexedColumn.asIdentifier().getCppObj()}, null);
        return this;
    }

    public Upsert indexedBy(String[] columnNames) {
        configIndexedColumn(cppObj, CPPType.String.ordinal(), null, columnNames);
        return this;
    }

    public Upsert indexedBy(IndexedColumnConvertible[] indexedColumns) {
        assert indexedColumns != null && indexedColumns.length > 0;
        if (indexedColumns == null || indexedColumns.length == 0) {
            return this;
        }

        long[] cppIndexedColumns = new long[indexedColumns.length];
        for(int i = 0; i < indexedColumns.length; i++) {
            cppIndexedColumns[i] = indexedColumns[i].asIdentifier().getCppObj();
        }

        configIndexedColumn(cppObj, indexedColumns[0].asIdentifier().getCppType().ordinal(), cppIndexedColumns, null);
        return this;
    }

    private native void configIndexedColumn(long self, int type, long[] columns, String[] columnNames);

    public Upsert where(Expression condition) {
        configWhere(cppObj, condition.getCppObj());
        return this;
    }

    private native void configWhere(long self, long condition);

    public Upsert doNoThing() {
        configDoNothing(cppObj);
        return this;
    }

    private native void configDoNothing(long self);

    public Upsert doUpdate() {
        configDoUpdate(cppObj);
        return this;
    }

    private native void configDoUpdate(long self);

    public Upsert setColumn(String columnName) {
        configSetColumns(cppObj, CPPType.String.ordinal(), null, new String[]{columnName});
        return this;
    }

    public Upsert setColumn(Column column) {
        assert column != null;
        if(column == null) {
            return this;
        }
        configSetColumns(cppObj, column.asIdentifier().getCppType().ordinal(), new long[]{column.asIdentifier().getCppObj()}, null);
        return this;
    }

    public Upsert setColumns(String[] columnNames) {
        configSetColumns(cppObj, CPPType.String.ordinal(), null, columnNames);
        return this;
    }

    public Upsert setColumns(Column[] columns) {
        assert columns != null && columns.length > 0;
        if (columns == null || columns.length == 0) {
            return this;
        }

        long[] cppColumns = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppColumns[i] = columns[i].asIdentifier().getCppObj();
        }

        configSetColumns(cppObj, columns[0].asIdentifier().getCppType().ordinal(), cppColumns, null);
        return this;
    }

    private native void configSetColumns(long self, int type, long[] columns, String[] columnNames);

    public Upsert to(boolean value) {
        configToValue(cppObj, CPPType.Bool.ordinal(), value ? 1 : 0, 0, null);
        return this;
    }

    public Upsert to(long value) {
        configToValue(cppObj, CPPType.Int.ordinal(), value, 0, null);
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
        if(value != null){
            configToValue(cppObj, value.asIdentifier().getCppType().ordinal(), value.asIdentifier().getCppObj(), 0, null);
        } else {
            configToValue(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }

        return this;
    }

    private native void configToValue(long self, int type, long intValue, double doubleValue, String stringValue);
}

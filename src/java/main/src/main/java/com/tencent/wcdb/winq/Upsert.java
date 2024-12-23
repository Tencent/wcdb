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

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class Upsert extends Identifier {
    @Override
    protected int getType() {
        return CPPType.UpsertClause;
    }

    public Upsert() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public Upsert onConflict() {
        return this;
    }

    @NotNull
    public Upsert indexedBy(@NotNull String... columnNames) {
        configIndexedColumn(cppObj, CPPType.String, null, columnNames);
        return this;
    }

    @NotNull
    public Upsert indexedBy(@NotNull IndexedColumnConvertible... indexedColumns) {
        if (indexedColumns.length == 0) {
            return this;
        }

        long[] cppIndexedColumns = new long[indexedColumns.length];
        for (int i = 0; i < indexedColumns.length; i++) {
            cppIndexedColumns[i] = CppObject.get(indexedColumns[i]);
        }

        configIndexedColumn(cppObj, Identifier.getCppType(indexedColumns[0]), cppIndexedColumns, null);
        return this;
    }

    private static native void configIndexedColumn(long self, int type, long[] columns, String[] columnNames);

    @NotNull
    public Upsert where(@Nullable Expression condition) {
        configWhere(cppObj, CppObject.get(condition));
        return this;
    }

    private static native void configWhere(long self, long condition);

    @NotNull
    public Upsert doNoThing() {
        configDoNothing(cppObj);
        return this;
    }

    private static native void configDoNothing(long self);

    @NotNull
    public Upsert doUpdate() {
        configDoUpdate(cppObj);
        return this;
    }

    private static native void configDoUpdate(long self);

    @NotNull
    public Upsert set(@NotNull String... columnNames) {
        configSetColumns(cppObj, CPPType.String, null, columnNames);
        return this;
    }

    @NotNull
    public Upsert set(@NotNull Column... columns) {
        if (columns.length == 0) {
            return this;
        }

        long[] cppColumns = new long[columns.length];
        for (int i = 0; i < columns.length; i++) {
            cppColumns[i] = CppObject.get(columns[i]);
        }

        configSetColumns(cppObj, Identifier.getCppType(columns[0]), cppColumns, null);
        return this;
    }

    private static native void configSetColumns(long self, int type, long[] columns, String[] columnNames);

    @NotNull
    public Upsert to(boolean value) {
        configToValue(cppObj, CPPType.Bool, value ? 1 : 0, 0, null);
        return this;
    }

    @NotNull
    public Upsert to(byte value) {
        configToValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public Upsert to(short value) {
        configToValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public Upsert to(int value) {
        configToValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public Upsert to(long value) {
        configToValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public Upsert to(float value) {
        configToValue(cppObj, CPPType.Double, 0, value, null);
        return this;
    }

    @NotNull
    public Upsert to(double value) {
        configToValue(cppObj, CPPType.Double, 0, value, null);
        return this;
    }

    @NotNull
    public Upsert to(@Nullable String value) {
        if (value != null) {
            configToValue(cppObj, CPPType.String, 0, 0, value);
        } else {
            configToValue(cppObj, CPPType.Null, 0, 0, null);
        }
        return this;
    }

    @NotNull
    public Upsert to(@Nullable ExpressionConvertible value) {
        configToValue(cppObj, Identifier.getCppType(value), CppObject.get(value), 0, null);
        return this;
    }

    private static native void configToValue(long self, int type, long intValue, double doubleValue, String stringValue);
}

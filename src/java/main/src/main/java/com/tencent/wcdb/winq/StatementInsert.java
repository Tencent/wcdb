// Created by qiuwenchen on 2023/4/12.
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

public class StatementInsert extends Statement {
    @Override
    protected int getType() {
        return CPPType.InsertSTMT;
    }

    public StatementInsert() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public StatementInsert with(@Nullable CommonTableExpression... expressions) {
        if (expressions == null || expressions.length == 0) {
            return this;
        }
        long[] cppExps = new long[expressions.length];
        for (int i = 0; i < expressions.length; i++) {
            cppExps[i] = CppObject.get(expressions[i]);
        }
        configWith(cppObj, cppExps);
        return this;
    }

    @NotNull
    public StatementInsert withRecursive(@Nullable CommonTableExpression... expressions) {
        if (expressions == null || expressions.length == 0) {
            return this;
        }
        long[] cppExps = new long[expressions.length];
        for (int i = 0; i < expressions.length; i++) {
            cppExps[i] = CppObject.get(expressions[i]);
        }
        configWith(cppObj, cppExps);
        configRecursive(cppObj);
        return this;
    }

    private static native void configWith(long self, long[] expressions);

    private static native void configRecursive(long self);

    @NotNull
    public StatementInsert insertInto(@NotNull String tableName) {
        configTableName(cppObj, tableName);
        return this;
    }

    private static native void configTableName(long self, String tableName);

    @NotNull
    public StatementInsert of(@Nullable String schemaName) {
        configSchema(cppObj, CPPType.String, 0, schemaName);
        return this;
    }

    @NotNull
    public StatementInsert of(@Nullable Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long schema, String schemaName);

    @NotNull
    public StatementInsert orReplace() {
        configConflictAction(cppObj, ConflictAction.Replace.ordinal());
        return this;
    }

    @NotNull
    public StatementInsert orRollback() {
        configConflictAction(cppObj, ConflictAction.Rollback.ordinal());
        return this;
    }

    @NotNull
    public StatementInsert orAbort() {
        configConflictAction(cppObj, ConflictAction.Abort.ordinal());
        return this;
    }

    @NotNull
    public StatementInsert orFail() {
        configConflictAction(cppObj, ConflictAction.Fail.ordinal());
        return this;
    }

    @NotNull
    public StatementInsert orIgnore() {
        configConflictAction(cppObj, ConflictAction.Ignore.ordinal());
        return this;
    }

    private static native void configConflictAction(long self, int action);

    @NotNull
    public StatementInsert as(@Nullable String alias) {
        configAlias(cppObj, alias);
        return this;
    }

    private static native void configAlias(long self, String alias);

    @NotNull
    public StatementInsert columns(@NotNull Column... columns) {
        if (columns.length == 0) {
            return this;
        }
        long[] cppColumns = new long[columns.length];
        for (int i = 0; i < columns.length; i++) {
            cppColumns[i] = CppObject.get(columns[i]);
        }
        configColumns(cppObj, CPPType.Column, cppColumns, null);
        return this;
    }

    @NotNull
    public StatementInsert columns(@NotNull String... columnNames) {
        if (columnNames.length == 0) {
            return this;
        }
        configColumns(cppObj, CPPType.String, null, columnNames);
        return this;
    }

    private static native void configColumns(long self, int type, long[] columns, String[] columnNames);

    @NotNull
    public StatementInsert valuesWithBindParameters(int parametersCount) {
        configValuesWithBindParameters(cppObj, parametersCount);
        return this;
    }

    private static native void configValuesWithBindParameters(long self, int parametersCount);

    @NotNull
    public StatementInsert values(@Nullable Object... values) {
        if (values == null || values.length == 0) {
            return this;
        }

        MultiTypeArray array = new MultiTypeArray(values);

        configValues(cppObj, array.types, array.longValues, array.doubleValues, array.stringValues);
        return this;
    }

    private static native void configValues(long self, int[] types, long[] longValues, double[] doubleValues, String[] stringValues);

    @NotNull
    public StatementInsert values(@NotNull StatementSelect select) {
        configValues(cppObj, CppObject.get(select));
        return this;
    }

    private static native void configValues(long self, long select);

    @NotNull
    public StatementInsert defaultValues() {
        configDefaultValues(cppObj);
        return this;
    }

    private static native void configDefaultValues(long self);

    @NotNull
    public StatementInsert upsert(@Nullable Upsert upsert) {
        configUpsert(cppObj, CppObject.get(upsert));
        return this;
    }

    private static native void configUpsert(long self, long upsert);
}

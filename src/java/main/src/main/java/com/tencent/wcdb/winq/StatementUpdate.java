// Created by chenqiuwen on 2023/4/13.
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

public class StatementUpdate extends Statement {
    @Override
    protected int getType() {
        return CPPType.UpdateSTMT;
    }

    public StatementUpdate() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public StatementUpdate with(@Nullable CommonTableExpression... expressions) {
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
    public StatementUpdate withRecursive(@Nullable CommonTableExpression... expressions) {
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
    public StatementUpdate update(@NotNull String tableName) {
        configTable(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public StatementUpdate update(@NotNull QualifiedTable table) {
        configTable(cppObj, Identifier.getCppType(table), CppObject.get(table), null);
        return this;
    }

    private static native void configTable(long self, int type, long table, String tableName);

    @NotNull
    public StatementUpdate orReplace() {
        configConflictAction(cppObj, ConflictAction.Replace.ordinal());
        return this;
    }

    @NotNull
    public StatementUpdate orRollback() {
        configConflictAction(cppObj, ConflictAction.Rollback.ordinal());
        return this;
    }

    @NotNull
    public StatementUpdate orAbort() {
        configConflictAction(cppObj, ConflictAction.Abort.ordinal());
        return this;
    }

    @NotNull
    public StatementUpdate orFail() {
        configConflictAction(cppObj, ConflictAction.Fail.ordinal());
        return this;
    }

    @NotNull
    public StatementUpdate orIgnore() {
        configConflictAction(cppObj, ConflictAction.Ignore.ordinal());
        return this;
    }

    private static native void configConflictAction(long self, int action);

    @NotNull
    public StatementUpdate setColumnsToBindParameters(@NotNull Column... columns) {
        if (columns.length == 0) {
            return this;
        }
        long[] cppObjs = new long[columns.length];
        for (int i = 0; i < columns.length; i++) {
            cppObjs[i] = CppObject.get(columns[i]);
        }
        configColumnsToBindParameters(cppObj, CPPType.Column, cppObjs, null);
        return this;
    }

    @NotNull
    public StatementUpdate setColumnsToBindParameters(@NotNull String... columnNames) {
        if (columnNames.length == 0) {
            return this;
        }
        configColumnsToBindParameters(cppObj, CPPType.String, null, columnNames);
        return this;
    }

    private static native void configColumnsToBindParameters(long self, int type, long[] columns, String[] columnNames);

    @NotNull
    public StatementUpdate setColumnsToValues(@NotNull Column[] columns, @NotNull Object[] values) {
        if (columns.length == 0 || values.length == 0) {
            return this;
        }

        long[] cppColumns = new long[columns.length];
        for (int i = 0; i < columns.length; i++) {
            cppColumns[i] = CppObject.get(columns[i]);
        }

        MultiTypeArray array = new MultiTypeArray(values);

        configColumnsToValues(cppObj, CPPType.Column, cppColumns, null,
                array.types, array.longValues, array.doubleValues, array.stringValues);

        return this;
    }

    @NotNull
    public StatementUpdate setColumnsToValues(@NotNull String[] columnNames, @NotNull Object[] values) {
        if (columnNames.length == 0 || values.length == 0) {
            return this;
        }

        MultiTypeArray array = new MultiTypeArray(values);

        configColumnsToValues(cppObj, CPPType.String, null, columnNames,
                array.types, array.longValues, array.doubleValues, array.stringValues);

        return this;
    }

    private static native void configColumnsToValues(long self,
                                                     int columnType,
                                                     long[] columns,
                                                     String[] columnNames,
                                                     int[] valueTypes,
                                                     long[] longValues,
                                                     double[] doubleValues,
                                                     String[] stringValues);

    @NotNull
    public StatementUpdate set(@NotNull Column... columns) {
        if (columns.length == 0) {
            return this;
        }
        long[] cppObjs = new long[columns.length];
        for (int i = 0; i < columns.length; i++) {
            cppObjs[i] = CppObject.get(columns[i]);
        }
        configColumns(cppObj, CPPType.Column, cppObjs, null);
        return this;
    }

    @NotNull
    public StatementUpdate set(@NotNull String... columnNames) {
        if (columnNames.length == 0) {
            return this;
        }
        configColumns(cppObj, CPPType.String, null, columnNames);
        return this;
    }

    private static native void configColumns(long self, int type, long[] columns, String[] columnNames);

    @NotNull
    public StatementUpdate to(boolean arg) {
        configToValue(cppObj, CPPType.Bool, arg ? 1 : 0, 0, null);
        return this;
    }

    @NotNull
    public StatementUpdate to(byte arg) {
        configToValue(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    @NotNull
    public StatementUpdate to(short arg) {
        configToValue(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    @NotNull
    public StatementUpdate to(int arg) {
        configToValue(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    @NotNull
    public StatementUpdate to(long arg) {
        configToValue(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    @NotNull
    public StatementUpdate to(float arg) {
        configToValue(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    @NotNull
    public StatementUpdate to(double arg) {
        configToValue(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    @NotNull
    public StatementUpdate to(@Nullable String arg) {
        if (arg != null) {
            configToValue(cppObj, CPPType.String, 0, 0, arg);
        } else {
            configToValue(cppObj, CPPType.Null, 0, 0, null);
        }
        return this;
    }

    @NotNull
    public StatementUpdate to(@Nullable ExpressionConvertible arg) {
        configToValue(cppObj, Identifier.getCppType(arg), CppObject.get(arg), 0, null);
        return this;
    }

    private static native void configToValue(long self, int type, long intValue, double doubleValue, String stringValue);

    @NotNull
    public StatementUpdate where(@Nullable Expression condition) {
        configCondition(cppObj, CppObject.get(condition));
        return this;
    }

    private static native void configCondition(long self, long condition);

    @NotNull
    public StatementUpdate orderBy(@Nullable OrderingTerm... orders) {
        if (orders == null || orders.length == 0) {
            return this;
        }
        long[] cppOrders = new long[orders.length];
        for (int i = 0; i < orders.length; i++) {
            cppOrders[i] = CppObject.get(orders[i]);
        }
        configOrders(cppObj, cppOrders);
        return this;
    }

    private static native void configOrders(long self, long[] orders);

    @NotNull
    public StatementUpdate limit(long from, long to) {
        configLimitRange(cppObj, CPPType.Int, from, CPPType.Int, to);
        return this;
    }

    @NotNull
    public StatementUpdate limit(long from, @Nullable ExpressionConvertible to) {
        configLimitRange(cppObj, CPPType.Int, from, Identifier.getCppType(to), CppObject.get(to));
        return this;
    }

    @NotNull
    public StatementUpdate limit(@Nullable ExpressionConvertible from, @Nullable ExpressionConvertible to) {
        configLimitRange(cppObj, Identifier.getCppType(from), CppObject.get(from), Identifier.getCppType(to), CppObject.get(to));
        return this;
    }

    @NotNull
    public StatementUpdate limit(@Nullable ExpressionConvertible from, long to) {
        configLimitRange(cppObj, Identifier.getCppType(from), CppObject.get(from), CPPType.Int, to);
        return this;
    }

    private static native void configLimitRange(long self, int fromType, long from, int toType, long to);

    @NotNull
    public StatementUpdate limit(long count) {
        configLimitCount(cppObj, CPPType.Int, count);
        return this;
    }

    @NotNull
    public StatementUpdate limit(@Nullable ExpressionConvertible count) {
        configLimitCount(cppObj, Identifier.getCppType(count), CppObject.get(count));
        return this;
    }

    private static native void configLimitCount(long self, int type, long count);

    @NotNull
    public StatementUpdate offset(long offset) {
        configOffset(cppObj, CPPType.Int, offset);
        return this;
    }

    @NotNull
    public StatementUpdate offset(@Nullable ExpressionConvertible offset) {
        configOffset(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configOffset(long self, int type, long count);
}

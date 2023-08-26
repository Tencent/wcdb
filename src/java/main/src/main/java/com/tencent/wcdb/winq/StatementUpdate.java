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

public class StatementUpdate extends Statement {
    @Override
    protected CPPType getType() {
        return CPPType.UpdateSTMT;
    }

    public StatementUpdate() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public StatementUpdate with(CommonTableExpression expression) {
        return with(new CommonTableExpression[]{expression});
    }

    public StatementUpdate with(CommonTableExpression[] expressions) {
        if(expressions == null || expressions.length == 0) {
            return this;
        }
        long[] cppExps = new long[expressions.length];
        for(int i = 0; i < expressions.length; i++) {
            cppExps[i] = CppObject.get(expressions[i]);
        }
        configWith(cppObj, cppExps);
        return this;
    }

    public StatementUpdate withRecursive(CommonTableExpression expression) {
        return withRecursive(new CommonTableExpression[]{expression});
    }

    public StatementUpdate withRecursive(CommonTableExpression[] expressions) {
        if(expressions == null || expressions.length == 0) {
            return this;
        }
        long[] cppExps = new long[expressions.length];
        for(int i = 0; i < expressions.length; i++) {
            cppExps[i] = CppObject.get(expressions[i]);
        }
        configWith(cppObj, cppExps);
        configRecursive(cppObj);
        return this;
    }

    private native void configWith(long self, long[] expressions);

    private native void configRecursive(long self);

    public StatementUpdate update(String tableName) {
        configTable(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public StatementUpdate update(QualifiedTable table) {
        configTable(cppObj, Identifier.getCppType(table), CppObject.get(table), null);
        return this;
    }

    private native void configTable(long self, int type, long table, String tableName);

    public StatementUpdate orReplace() {
        configConflictAction(cppObj, ConflictAction.Replace.ordinal());
        return this;
    }

    public StatementUpdate orRollback() {
        configConflictAction(cppObj, ConflictAction.Rollback.ordinal());
        return this;
    }

    public StatementUpdate orAbort() {
        configConflictAction(cppObj, ConflictAction.Abort.ordinal());
        return this;
    }

    public StatementUpdate orFail() {
        configConflictAction(cppObj, ConflictAction.Fail.ordinal());
        return this;
    }

    public StatementUpdate orIgnore() {
        configConflictAction(cppObj, ConflictAction.Ignore.ordinal());
        return this;
    }
    private native void configConflictAction(long self, int action);

    public StatementUpdate setColumnsToBindParameters(Column[] columns) {
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] cppObjs = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppObjs[i] = CppObject.get(columns[i]);
        }
        configColumnsToBindParameters(cppObj, CPPType.Column.ordinal(), cppObjs, null);
        return this;
    }

    public StatementUpdate setColumnsToBindParameters(String[] columnNames) {
        if(columnNames == null || columnNames.length == 0) {
            return this;
        }
        configColumnsToBindParameters(cppObj, CPPType.String.ordinal(), null, columnNames);
        return this;
    }

    private native void configColumnsToBindParameters(long self, int type, long[] columns, String[] columnNames);

    public StatementUpdate setColumnsToValues(Column[] columns, Object[] values) {
        if(columns == null || columns.length == 0 || values == null || values.length == 0) {
            return this;
        }

        long[] cppColumns = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppColumns[i] = CppObject.get(columns[i]);
        }

        MultiTypeArray array = new MultiTypeArray(values);

        configColumnsToValues(cppObj, CPPType.Column.ordinal(), cppColumns, null,
                array.types, array.longValues, array.doubleValues, array.stringValues);

        return this;
    }

    public StatementUpdate setColumnsToValues(String[] columnNames, Object[] values) {
        if(columnNames == null || columnNames.length == 0 || values == null || values.length == 0) {
            return this;
        }

        MultiTypeArray array = new MultiTypeArray(values);

        configColumnsToValues(cppObj, CPPType.String.ordinal(), null, columnNames,
                array.types, array.longValues, array.doubleValues, array.stringValues);

        return this;
    }

    private native void configColumnsToValues(long self,
                                              int columnType,
                                              long[] columns,
                                              String[] columnNames,
                                              int[] valueTypes,
                                              long[] longValues,
                                              double[] doubleValues,
                                              String[] stringValues);

    public StatementUpdate set(Column column) {
        configColumns(cppObj, Identifier.getCppType(column), new long[]{CppObject.get(column)}, null);
        return this;
    }

    public StatementUpdate set(Column[] columns) {
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] cppObjs = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppObjs[i] = CppObject.get(columns[i]);
        }
        configColumns(cppObj, CPPType.Column.ordinal(), cppObjs, null);
        return this;
    }

    public StatementUpdate set(String columnName) {
        if(columnName == null || columnName.length() == 0){
            return this;
        }
        configColumns(cppObj, CPPType.String.ordinal(), null, new String[]{columnName});
        return this;
    }

    public StatementUpdate set(String[] columnNames) {
        if(columnNames == null || columnNames.length == 0) {
            return this;
        }
        configColumns(cppObj, CPPType.String.ordinal(), null, columnNames);
        return this;
    }

    private native void configColumns(long self, int type, long[] columns, String[] columnNames);

    public StatementUpdate to(boolean arg) {
        configToValue(cppObj, CPPType.Bool.ordinal(), arg ? 1 : 0, 0, null);
        return this;
    }

    public StatementUpdate to(long arg) {
        configToValue(cppObj, CPPType.Int.ordinal(), arg, 0, null);
        return this;
    }

    public StatementUpdate to(double arg) {
        configToValue(cppObj, CPPType.Double.ordinal(), 0, arg, null);
        return this;
    }

    public StatementUpdate to(String arg) {
        if(arg != null) {
            configToValue(cppObj, CPPType.String.ordinal(), 0, 0, arg);
        } else {
            configToValue(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }
        return this;
    }

    public StatementUpdate to(ExpressionConvertible arg) {
        configToValue(cppObj, Identifier.getCppType(arg), CppObject.get(arg), 0, null);
        return this;
    }

    private native void configToValue(long self, int type, long intValue, double doubleValue, String stringValue);

    public StatementUpdate where(Expression condition) {
        configCondition(cppObj, CppObject.get(condition));
        return this;
    }

    private native void configCondition(long self, long condition);

    public StatementUpdate orderBy(OrderingTerm order) {
        configOrders(cppObj, new long[]{CppObject.get(order)});
        return this;
    }

    public StatementUpdate orderBy(OrderingTerm[] orders) {
        if(orders == null || orders.length == 0) {
            return this;
        }
        long[] cppOrders = new long[orders.length];
        for(int i = 0; i < orders.length; i++) {
            cppOrders[i] = CppObject.get(orders[i]);
        }
        configOrders(cppObj, cppOrders);
        return this;
    }

    private native void configOrders(long self, long[] orders);

    public StatementUpdate limit(long from, long to) {
        configLimitRange(cppObj, CPPType.Int.ordinal(), from, CPPType.Int.ordinal(), to);
        return this;
    }

    public StatementUpdate limit(long from, ExpressionConvertible to) {
        configLimitRange(cppObj, CPPType.Int.ordinal(), from, Identifier.getCppType(to), CppObject.get(to));
        return this;
    }

    public StatementUpdate limit(ExpressionConvertible from, ExpressionConvertible to) {
        configLimitRange(cppObj, Identifier.getCppType(from), CppObject.get(from), Identifier.getCppType(to), CppObject.get(to));
        return this;
    }

    public StatementUpdate limit(ExpressionConvertible from, long to) {
        configLimitRange(cppObj, Identifier.getCppType(from), CppObject.get(from), CPPType.Int.ordinal(), to);
        return this;
    }

    private native void configLimitRange(long self, int fromType, long from, int toType, long to);

    public StatementUpdate limit(long count) {
        configLimitCount(cppObj, CPPType.Int.ordinal(), count);
        return this;
    }

    public StatementUpdate limit(ExpressionConvertible count) {
        configLimitCount(cppObj, Identifier.getCppType(count), CppObject.get(count));
        return this;
    }

    private native void configLimitCount(long self, int type, long count);

    public StatementUpdate offset(long offset) {
        configOffset(cppObj, CPPType.Int.ordinal(), offset);
        return this;
    }

    public StatementUpdate offset(ExpressionConvertible offset) {
        configOffset(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private native void configOffset(long self, int type, long count);
}

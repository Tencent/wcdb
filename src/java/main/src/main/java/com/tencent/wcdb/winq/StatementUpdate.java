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

public class StatementUpdate extends Statement {
    @Override
    protected CPPType getCppType() {
        return CPPType.UpdateSTMT;
    }

    public StatementUpdate() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public StatementUpdate update(String tableName) {
        configTable(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public StatementUpdate update(QualifiedTable table) {
        configTable(cppObj, table.getCppType().ordinal(), table.getCppObj(), null);
        return this;
    }

    private native void configTable(long self, int type, long table, String tableName);

    public StatementUpdate orReplace() {
        configConflictAction(cppObj, ConflictAction.Replace.ordinal() + 1);
        return this;
    }

    public StatementUpdate orRollback() {
        configConflictAction(cppObj, ConflictAction.Rollback.ordinal() + 1);
        return this;
    }

    public StatementUpdate orAbort() {
        configConflictAction(cppObj, ConflictAction.Abort.ordinal() + 1);
        return this;
    }

    public StatementUpdate orFail() {
        configConflictAction(cppObj, ConflictAction.Fail.ordinal() + 1);
        return this;
    }

    public StatementUpdate orIgnore() {
        configConflictAction(cppObj, ConflictAction.Ignore.ordinal() + 1);
        return this;
    }
    private native void configConflictAction(long self, int action);

    public StatementUpdate setColumnsToBindParameters(Column[] columns) {
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] cppObjs = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppObjs[i] = columns[i].getCppObj();
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
            cppColumns[i] = columns[i].getCppObj();
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
        if(column == null) {
            return this;
        }
        configColumns(cppObj, CPPType.Column.ordinal(), new long[]{column.getCppObj()}, null);
        return this;
    }

    public StatementUpdate set(Column[] columns) {
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] cppObjs = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppObjs[i] = columns[i].getCppObj();
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
        if(arg != null) {
            configToValue(cppObj, arg.asIdentifier().getCppType().ordinal(), arg.asIdentifier().getCppObj(), 0, null);
        } else {
            configToValue(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }
        return this;
    }

    private native void configToValue(long self, int type, long intValue, double doubleValue, String stringValue);

    public StatementUpdate where(Expression condition) {
        configCondition(cppObj, condition.getCppObj());
        return this;
    }

    private native void configCondition(long self, long condition);

    public StatementUpdate orderBy(OrderingTerm order) {
        configOrders(cppObj, new long[]{order.getCppObj()});
        return this;
    }

    public StatementUpdate orderBy(OrderingTerm[] orders) {
        if(orders == null || orders.length == 0) {
            return this;
        }
        long[] cppOrders = new long[orders.length];
        for(int i = 0; i < orders.length; i++) {
            cppOrders[i] = orders[i].getCppObj();
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
        configLimitRange(cppObj, CPPType.Int.ordinal(), from, to.asIdentifier().getCppType().ordinal(), to.asIdentifier().getCppObj());
        return this;
    }

    public StatementUpdate limit(ExpressionConvertible from, ExpressionConvertible to) {
        configLimitRange(cppObj, from.asIdentifier().getCppType().ordinal(), from.asIdentifier().getCppObj(), to.asIdentifier().getCppType().ordinal(), to.asIdentifier().getCppObj());
        return this;
    }

    public StatementUpdate limit(ExpressionConvertible from, long to) {
        configLimitRange(cppObj, from.asIdentifier().getCppType().ordinal(), from.asIdentifier().getCppObj(), CPPType.Int.ordinal(), to);
        return this;
    }

    private native void configLimitRange(long self, int fromType, long from, int toType, long to);

    public StatementUpdate limit(long count) {
        configLimitCount(cppObj, CPPType.Int.ordinal(), count);
        return this;
    }

    public StatementUpdate limit(ExpressionConvertible count) {
        configLimitCount(cppObj, count.asIdentifier().getCppType().ordinal(), count.asIdentifier().getCppObj());
        return this;
    }

    private native void configLimitCount(long self, int type, long count);

    public StatementUpdate offset(long offset) {
        configOffset(cppObj, CPPType.Int.ordinal(), offset);
        return this;
    }

    public StatementUpdate offset(ExpressionConvertible offset) {
        configOffset(cppObj, offset.asIdentifier().getCppType().ordinal(), offset.asIdentifier().getCppObj());
        return this;
    }

    private native void configOffset(long self, int type, long count);
}

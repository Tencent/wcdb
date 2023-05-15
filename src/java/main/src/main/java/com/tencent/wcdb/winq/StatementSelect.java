// Created by chenqiuwen on 2023/4/1.
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

import com.tencent.wcdb.winq.Statement;

import java.util.Arrays;

public class StatementSelect extends Statement implements TableOrSubqueryConvertible {
    @Override
    protected CPPType getCppType() {
        return CPPType.SelectSTMT;
    }
    public StatementSelect() {
        cppObj = createCppObj();
    }
    private native long createCppObj();

    public StatementSelect select(ResultColumnConvertible resultColumn) {
        configResultColumns(cppObj, new int[]{resultColumn.asIdentifier().getCppType().ordinal()},
                new long[]{resultColumn.asIdentifier().getCppObj()}, null, null);
        return this;
    }

    public StatementSelect select(ResultColumnConvertible[] resultColumns) {
        if(resultColumns == null || resultColumns.length == 0) {
            return this;
        }
        int totalCount = resultColumns.length;
        int[] types = new int[totalCount];
        long[] cppObjs = new long[totalCount];
        for(int i = 0; i < totalCount; i++) {
            types[i] = resultColumns[i].asIdentifier().getCppType().ordinal();
            cppObjs[i] = resultColumns[i].asIdentifier().getCppObj();
        }
        configResultColumns(cppObj, types, cppObjs, null, null);
        return this;
    }

    public StatementSelect select(String resultColumn) {
        configResultColumns(cppObj, new int[]{CPPType.String.ordinal()}, null, null,
                new String[]{resultColumn});
        return this;
    }

    public StatementSelect select(String[] resultColumns) {
        if(resultColumns == null || resultColumns.length == 0) {
            return this;
        }
        int[] types = new int[resultColumns.length];
        Arrays.fill(types, CPPType.String.ordinal());
        configResultColumns(cppObj, types, null, null, resultColumns);
        return this;
    }

    public StatementSelect select(Object[] resultColumns) {
        if(resultColumns == null || resultColumns.length == 0) {
            return this;
        }
        int totalCount = resultColumns.length;
        int[] types = new int[totalCount];
        long[] cppObjs = new long[totalCount];
        String[] columnNames = new String[totalCount];
        int intIndex = 0;
        int stringIndex = 0;
        for(int i = 0; i < totalCount; i++) {
            Object obj = resultColumns[i];
            if(obj instanceof String){
                types[i] = CPPType.String.ordinal();
                columnNames[stringIndex] = (String) obj;
                stringIndex++;
            } else if (obj instanceof ResultColumnConvertible) {
                types[i] = ((ResultColumnConvertible)resultColumns[i]).asIdentifier().getCppType().ordinal();
                cppObjs[intIndex] = ((ResultColumnConvertible)resultColumns[i]).asIdentifier().getCppObj();
                intIndex++;
            }
        }
        if(columnNames.length * 0.75 > stringIndex) {
            if(stringIndex == 0) {
                columnNames = null;
            } else {
                columnNames = Arrays.copyOf(columnNames, stringIndex);
            }
        }
        configResultColumns(cppObj, types, cppObjs, null, columnNames);
        return this;
    }

    private native void configResultColumns(long self, int[] types, long[] columns, double[] unused, String[] columnNames);

    public StatementSelect distinct() {
        configDistinct(cppObj);
        return this;
    }

    private native void configDistinct(long self);

    public StatementSelect from(TableOrSubqueryConvertible tableOrSubquery) {
        configTableOrSubqueries(cppObj, new int[]{tableOrSubquery.asIdentifier().getCppType().ordinal()},
                new long[]{tableOrSubquery.asIdentifier().getCppObj()}, null, null);
        return this;
    }

    public StatementSelect from(TableOrSubqueryConvertible[] tableOrSubqueries) {
        if(tableOrSubqueries == null || tableOrSubqueries.length == 0) {
            return this;
        }
        int totalCount = tableOrSubqueries.length;
        int[] types = new int[totalCount];
        long[] cppObjs = new long[totalCount];
        for(int i = 0; i < totalCount; i++) {
            types[i] = tableOrSubqueries[i].asIdentifier().getCppType().ordinal();
            cppObjs[i] = tableOrSubqueries[i].asIdentifier().getCppObj();
        }
        configTableOrSubqueries(cppObj, types, cppObjs, null, null);
        return this;
    }

    public StatementSelect from(String tableName) {
        configTableOrSubqueries(cppObj, new int[]{CPPType.String.ordinal()}, null, null, new String[]{tableName});
        return this;
    }

    public StatementSelect from(String[] tableNames) {
        if(tableNames == null || tableNames.length == 0) {
            return this;
        }
        int[] types = new int[tableNames.length];
        Arrays.fill(types, CPPType.String.ordinal());
        configTableOrSubqueries(cppObj, types, null, null, tableNames);
        return this;
    }

    public StatementSelect from(Object[] tableOrSubqueries) {
        if(tableOrSubqueries == null || tableOrSubqueries.length == 0) {
            return this;
        }
        int totalCount = tableOrSubqueries.length;
        int[] types = new int[totalCount];
        long[] cppObjs = new long[totalCount];
        String[] tablNames = new String[totalCount];
        int intIndex = 0;
        int stringIndex = 0;
        for(int i = 0; i < totalCount; i++) {
            Object obj = tableOrSubqueries[i];
            if(obj instanceof String){
                types[i] = CPPType.String.ordinal();
                tablNames[stringIndex] = (String) obj;
                stringIndex++;
            } else if (obj instanceof TableOrSubqueryConvertible) {
                types[i] = ((TableOrSubqueryConvertible)tableOrSubqueries[i]).asIdentifier().getCppType().ordinal();
                cppObjs[intIndex] = ((TableOrSubqueryConvertible)tableOrSubqueries[i]).asIdentifier().getCppObj();
                intIndex++;
            }
        }
        if(tablNames.length * 0.75 > stringIndex) {
            if(stringIndex == 0) {
                tablNames = null;
            } else {
                tablNames = Arrays.copyOf(tablNames, stringIndex);
            }
        }
        configTableOrSubqueries(cppObj, types, cppObjs, null, tablNames);
        return this;
    }

    private native void configTableOrSubqueries(long self, int[] types, long[] tableOrSubqueries, double[] unused, String[] tableNames);

    public StatementSelect where(Expression condition) {
        configCondition(cppObj, condition.getCppObj());
        return this;
    }

    private native void configCondition(long self, long condition);

    public StatementSelect groupBy(ExpressionConvertible expression) {
        configGroups(cppObj, new int[]{expression.asIdentifier().getCppType().ordinal()},
                new long[]{expression.asIdentifier().getCppObj()}, null, null);
        return this;
    }

    public StatementSelect groupBy(ExpressionConvertible[] expressions) {
        if(expressions == null || expressions.length == 0) {
            return this;
        }
        int totalCount = expressions.length;
        int[] types = new int[totalCount];
        long[] cppObjs = new long[totalCount];
        for(int i = 0; i < totalCount; i++) {
            types[i] = expressions[i].asIdentifier().getCppType().ordinal();
            cppObjs[i] = expressions[i].asIdentifier().getCppObj();
        }
        configGroups(cppObj, types, cppObjs, null, null);
        return this;
    }

    public StatementSelect groupBy(String columnName) {
        configGroups(cppObj, new int[]{CPPType.String.ordinal()}, null, null, new String[]{columnName});
        return this;
    }

    public StatementSelect groupBy(String[] columnNames) {
        if(columnNames == null || columnNames.length == 0) {
            return this;
        }
        int[] types = new int[columnNames.length];
        Arrays.fill(types, CPPType.String.ordinal());
        configGroups(cppObj, types, null, null, columnNames);
        return this;
    }

    public StatementSelect groupBy(Object[] expressions) {
        if(expressions == null || expressions.length == 0) {
            return this;
        }
        int totalCount = expressions.length;
        int[] types = new int[totalCount];
        long[] cppObjs = new long[totalCount];
        String[] columnNames = new String[totalCount];
        int intIndex = 0;
        int stringIndex = 0;
        for(int i = 0; i < totalCount; i++) {
            Object obj = expressions[i];
            if(obj instanceof String){
                types[i] = CPPType.String.ordinal();
                columnNames[stringIndex] = (String) obj;
                stringIndex++;
            } else if (obj instanceof ExpressionConvertible) {
                types[i] = ((ExpressionConvertible)expressions[i]).asIdentifier().getCppType().ordinal();
                cppObjs[intIndex] = ((ExpressionConvertible)expressions[i]).asIdentifier().getCppObj();
                intIndex++;
            }
        }
        if(columnNames.length * 0.75 > stringIndex) {
            if(stringIndex == 0) {
                columnNames = null;
            } else {
                columnNames = Arrays.copyOf(columnNames, stringIndex);
            }
        }
        configGroups(cppObj, types, cppObjs, null, columnNames);
        return this;
    }

    private native void configGroups(long self, int[] types, long[] exps, double[] unused, String[] columNames);

    public StatementSelect having(Expression expression) {
        configHaving(cppObj, expression.getCppObj());
        return this;
    }

    private native void configHaving(long self, long expression);

    public StatementSelect union() {
        configUnion(cppObj);
        return this;
    }

    private native void configUnion(long self);

    public StatementSelect unionAll() {
        configUnionAll(cppObj);
        return this;
    }

    private native void configUnionAll(long self);

    public StatementSelect intersect() {
        configIntersect(cppObj);
        return this;
    }

    private native void configIntersect(long self);

    public StatementSelect except() {
        configExcept(cppObj);
        return this;
    }

    private native void configExcept(long self);

    public StatementSelect orderBy(OrderingTerm order) {
        configOrders(cppObj, new long[]{order.getCppObj()});
        return this;
    }

    public StatementSelect orderBy(OrderingTerm[] orders) {
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

    public StatementSelect limit(long from, long to) {
        configLimitRange(cppObj, CPPType.Int.ordinal(), from, CPPType.Int.ordinal(), to);
        return this;
    }

    public StatementSelect limit(long from, ExpressionConvertible to) {
        configLimitRange(cppObj, CPPType.Int.ordinal(), from, to.asIdentifier().getCppType().ordinal(), to.asIdentifier().getCppObj());
        return this;
    }

    public StatementSelect limit(ExpressionConvertible from, ExpressionConvertible to) {
        configLimitRange(cppObj, from.asIdentifier().getCppType().ordinal(), from.asIdentifier().getCppObj(), to.asIdentifier().getCppType().ordinal(), to.asIdentifier().getCppObj());
        return this;
    }

    public StatementSelect limit(ExpressionConvertible from, long to) {
        configLimitRange(cppObj, from.asIdentifier().getCppType().ordinal(), from.asIdentifier().getCppObj(), CPPType.Int.ordinal(), to);
        return this;
    }

    private native void configLimitRange(long self, int fromType, long from, int toType, long to);

    public StatementSelect limit(long count) {
        configLimitCount(cppObj, CPPType.Int.ordinal(), count);
        return this;
    }

    public StatementSelect limit(ExpressionConvertible count) {
        configLimitCount(cppObj, count.asIdentifier().getCppType().ordinal(), count.asIdentifier().getCppObj());
        return this;
    }

    private native void configLimitCount(long self, int type, long count);

    public StatementSelect offset(long offset) {
        configOffset(cppObj, CPPType.Int.ordinal(), offset);
        return this;
    }

    public StatementSelect offset(ExpressionConvertible offset) {
        configOffset(cppObj, offset.asIdentifier().getCppType().ordinal(), offset.asIdentifier().getCppObj());
        return this;
    }

    private native void configOffset(long self, int type, long count);
}


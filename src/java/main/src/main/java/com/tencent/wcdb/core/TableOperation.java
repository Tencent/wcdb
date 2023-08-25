// Created by chenqiuwen on 2023/5/3.
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

package com.tencent.wcdb.core;

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.ConflictAction;
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.OrderingTerm;
import com.tencent.wcdb.winq.ResultColumnConvertible;
import com.tencent.wcdb.winq.Statement;
import com.tencent.wcdb.winq.StatementDelete;
import com.tencent.wcdb.winq.StatementInsert;
import com.tencent.wcdb.winq.StatementSelect;
import com.tencent.wcdb.winq.StatementUpdate;

import java.util.List;

public class TableOperation {
    String tableName = null;
    Database database = null;
    TableOperation(){
    }

    public String getTableName() {
        return tableName;
    }

    public Database getDatabase() {
        return database;
    }

    public void insertRow(Value[] row, Column[] columns) throws WCDBException {
        insertRows(new Value[][]{row}, columns);
    }

    public void insertRows(Value[][] rows, Column[] columns) throws WCDBException {
        insertRows(rows, columns, ConflictAction.None);
    }

    public void insertOrReplaceRow(Value[] row, Column[] columns) throws WCDBException {
        insertOrReplaceRows(new Value[][]{row}, columns);
    }

    public void insertOrReplaceRows(Value[][] rows, Column[] columns) throws WCDBException {
        insertRows(rows, columns, ConflictAction.Replace);
    }

    public void insertOrIgnoreRow(Value[] row, Column[] columns) throws WCDBException {
        insertOrIgnoreRows(new Value[][]{row}, columns);
    }

    public void insertOrIgnoreRows(Value[][] rows, Column[] columns) throws WCDBException {
        insertRows(rows, columns, ConflictAction.Ignore);
    }

    private void insertRows(final Value[][] rows, Column[] columns, ConflictAction action) throws WCDBException {
        final StatementInsert insert = new StatementInsert().insertInto(tableName).columns(columns).valuesWithBindParameters(columns.length);
        if(action == ConflictAction.Replace) {
            insert.orReplace();
        } else if (action == ConflictAction.Ignore) {
            insert.orIgnore();
        }
        Handle handle = database.getHandle(true);
        try {
            if(rows.length > 1) {
                handle.runTransaction(new Transaction() {
                    @Override
                    public boolean insideTransaction(Handle handle) throws WCDBException {
                        insertRows(rows, insert, handle);
                        return true;
                    }
                });
            } else {
                insertRows(rows, insert, handle);
            }
        } finally {
            handle.invalidate();
        }
    }

    private void insertRows(Value[][] rows, StatementInsert insert, Handle handle) throws WCDBException {
        PreparedStatement preparedStatement = handle.preparedWithMainStatement(insert);
        for(Value[] row : rows) {
            preparedStatement.reset();
            preparedStatement.bindRow(row);
            preparedStatement.step();
        }
        preparedStatement.finalizeStatement();
    }

    public void updateValue(long value, Column column) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column});
    }

    public void updateValue(long value, Column column, Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition);
    }

    public void updateValue(long value, Column column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(long value, Column column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(long value, Column column, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit);
    }

    public void updateValue(long value, Column column, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit, offset);
    }

    public void updateValue(double value, Column column) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column});
    }

    public void updateValue(double value, Column column, Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition);
    }

    public void updateValue(double value, Column column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(double value, Column column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(double value, Column column, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit);
    }

    public void updateValue(double value, Column column, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit, offset);
    }

    public void updateValue(String value, Column column) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column});
    }

    public void updateValue(String value, Column column, Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition);
    }

    public void updateValue(String value, Column column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(String value, Column column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(String value, Column column, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit);
    }

    public void updateValue(String value, Column column, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit, offset);
    }

    public void updateValue(byte[] value, Column column) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column});
    }

    public void updateValue(byte[] value, Column column, Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition);
    }

    public void updateValue(byte[] value, Column column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(byte[] value, Column column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(byte[] value, Column column, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit);
    }

    public void updateValue(byte[] value, Column column, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit, offset);
    }

    public void updateValue(Value value, Column column) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column});
    }

    public void updateValue(Value value, Column column, Expression condition) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, condition);
    }

    public void updateValue(Value value, Column column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(Value value, Column column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(Value value, Column column, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, order, limit);
    }

    public void updateValue(Value value, Column column, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, order, limit, offset);
    }

    public void updateRow(Value[] row, Column[] columns) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns);
        updateRow(row, update);
    }

    public void updateRow(Value[] row, Column[] columns, Expression condition) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition);
        updateRow(row, update);
    }

    public void updateRow(Value[] row, Column[] columns, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition).orderBy(order).limit(limit);
        updateRow(row, update);
    }

    public void updateRow(Value[] row, Column[] columns, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition).orderBy(order).limit(limit).offset(offset);
        updateRow(row, update);
    }

    public void updateRow(Value[] row, Column[] columns, OrderingTerm order, long limit) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).orderBy(order).limit(limit);
        updateRow(row, update);
    }

    public void updateRow(Value[] row, Column[] columns, OrderingTerm order, long limit, long offset) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).orderBy(order).limit(limit).offset(offset);
        updateRow(row, update);
    }

    private void updateRow(Value[] row, StatementUpdate update) throws WCDBException {
        Handle handle = database.getHandle(true);
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(update);
            preparedStatement.bindRow(row);
            preparedStatement.step();
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
    }

    public void deleteValue() throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName);
        deleteValue(delete);
    }

    public void deleteValue(Expression condition) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition);
        deleteValue(delete);
    }

    public void deleteValue(Expression condition, OrderingTerm orderingTerm, long limit) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition).orderBy(orderingTerm).limit(limit);
        deleteValue(delete);
    }

    public void deleteValue(Expression condition, OrderingTerm orderingTerm, long limit, long offset) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition).orderBy(orderingTerm).limit(limit).offset(offset);
        deleteValue(delete);
    }

    public void deleteValue(OrderingTerm orderingTerm, long limit) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).orderBy(orderingTerm).limit(limit);
        deleteValue(delete);
    }

    public void deleteValue(OrderingTerm orderingTerm, long limit, long offset) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).orderBy(orderingTerm).limit(limit).offset(offset);
        deleteValue(delete);
    }

    private void deleteValue(StatementDelete delete) throws WCDBException {
        Handle handle = database.getHandle(true);
        try {
            handle.execute(delete);
        } finally {
            handle.invalidate();
        }
    }

    public Value getValue(ResultColumnConvertible column) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName));
    }

    public Value getValue(ResultColumnConvertible column, Expression condition) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    public Value getValue(ResultColumnConvertible column, Expression condition, OrderingTerm order) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    public Value getValue(ResultColumnConvertible column, Expression condition, OrderingTerm order, long offset) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(1).offset(offset));
    }

    public Value getValue(ResultColumnConvertible column, OrderingTerm order) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    public Value getValue(ResultColumnConvertible column, OrderingTerm order, long offset) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(1).offset(offset));
    }

    public List<Value> getOneColumn(ResultColumnConvertible column) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName));
    }

    public List<Value> getOneColumn(ResultColumnConvertible column, Expression condition) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    public List<Value> getOneColumn(ResultColumnConvertible column, Expression condition, OrderingTerm order) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    public List<Value> getOneColumn(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    public List<Value> getOneColumn(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public List<Value> getOneColumn(ResultColumnConvertible column, OrderingTerm order) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    public List<Value> getOneColumn(ResultColumnConvertible column, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    public List<Value> getOneColumn(ResultColumnConvertible column, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, Expression condition) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, Expression condition, OrderingTerm orderingTerm) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(orderingTerm));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, Expression condition, OrderingTerm orderingTerm, long offset) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(orderingTerm).limit(1).offset(offset));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, OrderingTerm orderingTerm) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(orderingTerm));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, OrderingTerm orderingTerm, long offset) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(orderingTerm).limit(1).offset(offset));
    }

    public List<Integer> getOneColumnInt(ResultColumnConvertible column) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName));
    }

    public List<Integer> getOneColumnInt(ResultColumnConvertible column, Expression condition) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    public List<Integer> getOneColumnInt(ResultColumnConvertible column, Expression condition, OrderingTerm order) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    public List<Integer> getOneColumnInt(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    public List<Integer> getOneColumnInt(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public List<Integer> getOneColumnInt(ResultColumnConvertible column, OrderingTerm order) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    public List<Integer> getOneColumnInt(ResultColumnConvertible column, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    public List<Integer> getOneColumnInt(ResultColumnConvertible column, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    public List<Long> getOneColumnLong(ResultColumnConvertible column) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName));
    }

    public List<Long> getOneColumnLong(ResultColumnConvertible column, Expression condition) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    public List<Long> getOneColumnLong(ResultColumnConvertible column, Expression condition, OrderingTerm order) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    public List<Long> getOneColumnLong(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    public List<Long> getOneColumnLong(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public List<Long> getOneColumnLong(ResultColumnConvertible column, OrderingTerm order) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    public List<Long> getOneColumnLong(ResultColumnConvertible column, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    public List<Long> getOneColumnLong(ResultColumnConvertible column, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    public List<Float> getOneColumnFloat(ResultColumnConvertible column) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName));
    }

    public List<Float> getOneColumnFloat(ResultColumnConvertible column, Expression condition) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    public List<Float> getOneColumnFloat(ResultColumnConvertible column, Expression condition, OrderingTerm order) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    public List<Float> getOneColumnFloat(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    public List<Float> getOneColumnFloat(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public List<Float> getOneColumnFloat(ResultColumnConvertible column, OrderingTerm order) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    public List<Float> getOneColumnFloat(ResultColumnConvertible column, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    public List<Float> getOneColumnFloat(ResultColumnConvertible column, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    public List<Double> getOneColumnDouble(ResultColumnConvertible column) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName));
    }

    public List<Double> getOneColumnDouble(ResultColumnConvertible column, Expression condition) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    public List<Double> getOneColumnDouble(ResultColumnConvertible column, Expression condition, OrderingTerm order) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    public List<Double> getOneColumnDouble(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    public List<Double> getOneColumnDouble(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public List<Double> getOneColumnDouble(ResultColumnConvertible column, OrderingTerm order) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    public List<Double> getOneColumnDouble(ResultColumnConvertible column, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    public List<Double> getOneColumnDouble(ResultColumnConvertible column, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    public List<String> getOneColumnString(ResultColumnConvertible column) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName));
    }

    public List<String> getOneColumnString(ResultColumnConvertible column, Expression condition) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    public List<String> getOneColumnString(ResultColumnConvertible column, Expression condition, OrderingTerm order) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    public List<String> getOneColumnString(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    public List<String> getOneColumnString(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public List<String> getOneColumnString(ResultColumnConvertible column, OrderingTerm order) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    public List<String> getOneColumnString(ResultColumnConvertible column, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    public List<String> getOneColumnString(ResultColumnConvertible column, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    public List<byte[]> getOneColumnBLOB(ResultColumnConvertible column) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName));
    }

    public List<byte[]> getOneColumnBLOB(ResultColumnConvertible column, Expression condition) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    public List<byte[]> getOneColumnBLOB(ResultColumnConvertible column, Expression condition, OrderingTerm order) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    public List<byte[]> getOneColumnBLOB(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    public List<byte[]> getOneColumnBLOB(ResultColumnConvertible column, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public List<byte[]> getOneColumnBLOB(ResultColumnConvertible column, OrderingTerm order) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    public List<byte[]> getOneColumnBLOB(ResultColumnConvertible column, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    public List<byte[]> getOneColumnBLOB(ResultColumnConvertible column, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    public List<Value[]> getAllRows(ResultColumnConvertible[] columns) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName));
    }

    public List<Value[]> getAllRows(ResultColumnConvertible[] columns, Expression condition) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition));
    }

    public List<Value[]> getAllRows(ResultColumnConvertible[] columns, Expression condition, OrderingTerm order) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order));
    }

    public List<Value[]> getAllRows(ResultColumnConvertible[] columns, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    public List<Value[]> getAllRows(ResultColumnConvertible[] columns, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public List<Value[]> getAllRows(ResultColumnConvertible[] columns, OrderingTerm order) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order));
    }

    public List<Value[]> getAllRows(ResultColumnConvertible[] columns, OrderingTerm order, long limit) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order).limit(limit));
    }

    public List<Value[]> getAllRows(ResultColumnConvertible[] columns, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    public Value getValueFromStatement(Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        Value ret = null;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            preparedStatement.step();
            if(!preparedStatement.isDone()) {
                ret = preparedStatement.getValue(0);
            }
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
        return ret;
    }

    public Value[] getOneRowFromStatement(Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        Value[] ret = null;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            preparedStatement.step();
            if(!preparedStatement.isDone()) {
                ret = preparedStatement.getOneRow();
            }
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
        return ret;
    }

    public List<Value> getOneColumnFromStatement(Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Value> ret;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumn();
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
        return ret;
    }
    public List<Integer> getOneColumnIntFromStatement(Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Integer> ret;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnInt();
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
        return ret;
    }

    public List<Long> getOneColumnLongFromStatement(Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Long> ret;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnLong();
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
        return ret;
    }

    public List<Float> getOneColumnFloatFromStatement(Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Float> ret;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnFloat();
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
        return ret;
    }

    public List<Double> getOneColumnDoubleFromStatement(Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Double> ret;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnDouble();
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
        return ret;
    }

    public List<String> getOneColumnStringFromStatement(Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<String> ret;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnString();
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
        return ret;
    }

    public List<byte[]> getOneColumnBLOBFromStatement(Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<byte[]> ret;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnBLOB();
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
        return ret;
    }

    public List<Value[]> getAllRowsFromStatement(Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Value[]> ret;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getMultiRows();
            preparedStatement.finalizeStatement();
        } finally {
            handle.invalidate();
        }
        return ret;
    }
}
// Created by qiuwenchen on 2023/3/30.
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

import com.tencent.wcdb.base.CppObject;
import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.orm.Field;
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

import java.util.ArrayList;

public abstract class HandleOperation extends CppObject {
    abstract Handle getHandle();

    abstract boolean autoInvalidateHandle();

    public void insertRow(Value[] row, Field[] fields) throws WCDBException {
        insertRows(new Value[][]{row}, fields);
    }

    public void insertRows(Value[][] rows, Field[] fields) throws WCDBException {
        insertRows(rows, fields, Field.getTableName(fields), ConflictAction.None);
    }

    public void insertRow(Value[] row, Column[] columns, String tableName) throws WCDBException {
        insertRows(new Value[][]{row}, columns, tableName);
    }

    public void insertRows(Value[][] rows, Column[] columns, String tableName) throws WCDBException {
        insertRows(rows, columns, tableName, ConflictAction.None);
    }

    public void insertOrReplaceRow(Value[] row, Field[] fields) throws WCDBException {
        insertOrReplaceRows(new Value[][]{row}, fields);
    }

    public void insertOrReplaceRows(Value[][] rows, Field[] fields) throws WCDBException {
        insertRows(rows, fields, Field.getTableName(fields), ConflictAction.Replace);
    }

    public void insertOrReplaceRow(Value[] row, Column[] columns, String tableName) throws WCDBException {
        insertOrReplaceRows(new Value[][]{row}, columns, tableName);
    }

    public void insertOrReplaceRows(Value[][] rows, Column[] columns, String tableName) throws WCDBException {
        insertRows(rows, columns, tableName, ConflictAction.Replace);
    }

    public void insertOrIgnoreRow(Value[] row, Field[] fields) throws WCDBException {
        insertOrIgnoreRows(new Value[][]{row}, fields);
    }

    public void insertOrIgnoreRows(Value[][] rows, Field[] fields) throws WCDBException {
        insertRows(rows, fields, Field.getTableName(fields), ConflictAction.Ignore);
    }

    public void insertOrIgnoreRow(Value[] row, Column[] columns, String tableName) throws WCDBException {
        insertOrIgnoreRows(new Value[][]{row}, columns, tableName);
    }

    public void insertOrIgnoreRows(Value[][] rows, Column[] columns, String tableName) throws WCDBException {
        insertRows(rows, columns, tableName, ConflictAction.Ignore);
    }

    private void insertRows(Value[][] rows, Column[] columns, String tableName, ConflictAction action) throws WCDBException {
        StatementInsert insert = new StatementInsert().insertInto(tableName).columns(columns).valuesWithBindParameters(columns.length);
        if(action == ConflictAction.Replace) {
            insert.orReplace();
        } else if (action == ConflictAction.Ignore) {
            insert.orIgnore();
        }
        Handle handle = getHandle();
        try {
            if(rows.length > 1) {
                handle.runTransaction(new Handle.Transaction() {
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
            if(autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
    }

    private void insertRows(Value[][] rows, StatementInsert insert, Handle handle) throws WCDBException {
        PreparedStatement preparedStatement = handle.preparedWithMainStatement(insert);
        for(Value[] row : rows) {
            preparedStatement.reset();
            preparedStatement.bindRow(row);
            preparedStatement.step();
        }
        preparedStatement.finalize();
    }

    public void updateValue(long value, Field field) throws WCDBException {
        updateValue(value, field, Field.getTableName(field));
    }

    public void updateValue(long value, Column column, String tableName) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName);
    }

    public void updateValue(long value, Field field, Expression condition) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition);
    }

    public void updateValue(long value, Column column, String tableName, Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition);
    }

    public void updateValue(long value, Field field, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition, order, limit);
    }

    public void updateValue(long value, Column column, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit);
    }

    public void updateValue(long value, Field field, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition, order, limit, offset);
    }

    public void updateValue(long value, Column column, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit, offset);
    }

    public void updateValue(long value, Field field, OrderingTerm order, long limit) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), order, limit);
    }

    public void updateValue(long value, Column column, String tableName, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit);
    }

    public void updateValue(long value, Field field, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), order, limit, offset);
    }

    public void updateValue(long value, Column column, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit, offset);
    }

    public void updateValue(double value, Field field) throws WCDBException {
        updateValue(value, field, Field.getTableName(field));
    }

    public void updateValue(double value, Column column, String tableName) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName);
    }

    public void updateValue(double value, Field field, Expression condition) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition);
    }

    public void updateValue(double value, Column column, String tableName, Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition);
    }

    public void updateValue(double value, Field field, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition, order, limit);
    }

    public void updateValue(double value, Column column, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit);
    }

    public void updateValue(double value, Field field, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition, order, limit, offset);
    }

    public void updateValue(double value, Column column, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit, offset);
    }

    public void updateValue(double value, Field field, OrderingTerm order, long limit) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), order, limit);
    }

    public void updateValue(double value, Column column, String tableName, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit);
    }

    public void updateValue(double value, Field field, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), order, limit, offset);
    }

    public void updateValue(double value, Column column, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit, offset);
    }

    public void updateValue(String value, Field field) throws WCDBException {
        updateValue(value, field, Field.getTableName(field));
    }

    public void updateValue(String value, Column column, String tableName) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName);
    }

    public void updateValue(String value, Field field, Expression condition) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition);
    }

    public void updateValue(String value, Column column, String tableName, Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition);
    }

    public void updateValue(String value, Field field, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition, order, limit);
    }

    public void updateValue(String value, Column column, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit);
    }

    public void updateValue(String value, Field field, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition, order, limit, offset);
    }

    public void updateValue(String value, Column column, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit, offset);
    }

    public void updateValue(String value, Field field, OrderingTerm order, long limit) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), order, limit);
    }

    public void updateValue(String value, Column column, String tableName, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit);
    }

    public void updateValue(String value, Field field, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), order, limit, offset);
    }

    public void updateValue(String value, Column column, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit, offset);
    }

    public void updateValue(byte[] value, Field field) throws WCDBException {
        updateValue(value, field, Field.getTableName(field));
    }

    public void updateValue(byte[] value, Column column, String tableName) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName);
    }

    public void updateValue(byte[] value, Field field, Expression condition) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition);
    }

    public void updateValue(byte[] value, Column column, String tableName, Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition);
    }

    public void updateValue(byte[] value, Field field, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition, order, limit);
    }

    public void updateValue(byte[] value, Column column, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit);
    }

    public void updateValue(byte[] value, Field field, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition, order, limit, offset);
    }

    public void updateValue(byte[] value, Column column, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit, offset);
    }

    public void updateValue(byte[] value, Field field, OrderingTerm order, long limit) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), order, limit);
    }

    public void updateValue(byte[] value, Column column, String tableName, OrderingTerm order, long limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit);
    }

    public void updateValue(byte[] value, Field field, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), order, limit, offset);
    }

    public void updateValue(byte[] value, Column column, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit, offset);
    }

    public void updateValue(Value value, Field field) throws WCDBException {
        updateValue(value, field, Field.getTableName(field));
    }

    public void updateValue(Value value, Column column, String tableName) throws WCDBException {
        if(value == null) {
            value = new Value();
        }
        updateRow(new Value[]{value}, new Column[]{column}, tableName);
    }

    public void updateValue(Value value, Field field, Expression condition) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition);
    }

    public void updateValue(Value value, Column column, String tableName, Expression condition) throws WCDBException {
        if(value == null) {
            value = new Value();
        }
        updateRow(new Value[]{value}, new Column[]{column}, tableName, condition);
    }

    public void updateValue(Value value, Field field, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition, order, limit);
    }

    public void updateValue(Value value, Column column, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        if(value == null) {
            value = new Value();
        }
        updateRow(new Value[]{value}, new Column[]{column}, tableName, condition, order, limit);
    }

    public void updateValue(Value value, Field field, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), condition, order, limit, offset);
    }

    public void updateValue(Value value, Column column, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, tableName, condition, order, limit, offset);
    }

    public void updateValue(Value value, Field field, OrderingTerm order, long limit) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), order, limit);
    }

    public void updateValue(Value value, Column column, String tableName, OrderingTerm order, long limit) throws WCDBException {
        if(value == null) {
            value = new Value();
        }
        updateRow(new Value[]{value}, new Column[]{column}, tableName, order, limit);
    }

    public void updateValue(Value value, Field field, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateValue(value, field, Field.getTableName(field), order, limit, offset);
    }

    public void updateValue(Value value, Column column, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, tableName, order, limit, offset);
    }

    public void updateRow(Value[] row, Field[] fields) throws WCDBException {
        updateRow(row, fields, Field.getTableName(fields));
    }

    public void updateRow(Value[] row, Column[] columns, String tableName) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns);
        updateRow(row, update);
    }

    public void updateRow(Value[] row, Field[] fields, Expression condition) throws WCDBException {
        updateRow(row, fields, Field.getTableName(fields), condition);
    }

    public void updateRow(Value[] row, Column[] columns, String tableName, Expression condition) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition);
        updateRow(row, update);
    }

    public void updateRow(Value[] row, Field[] fields, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        updateRow(row, fields, Field.getTableName(fields), condition, order, limit);
    }

    public void updateRow(Value[] row, Column[] columns, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition).orderBy(order).limit(limit);
        updateRow(row, update);
    }

    public void updateRow(Value[] row, Field[] fields, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(row, fields, Field.getTableName(fields), condition, order, limit, offset);
    }

    public void updateRow(Value[] row, Column[] columns, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition).orderBy(order).limit(limit).offset(offset);
        updateRow(row, update);
    }

    public void updateRow(Value[] row, Field[] fields, OrderingTerm order, long limit) throws WCDBException {
        updateRow(row, fields, Field.getTableName(fields), order, limit);
    }

    public void updateRow(Value[] row, Column[] columns, String tableName, OrderingTerm order, long limit) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).orderBy(order).limit(limit);
        updateRow(row, update);
    }

    public void updateRow(Value[] row, Field[] fields, OrderingTerm order, long limit, long offset) throws WCDBException {
        updateRow(row, fields, Field.getTableName(fields), order, limit, offset);
    }

    public void updateRow(Value[] row, Column[] columns, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).orderBy(order).limit(limit).offset(offset);
        updateRow(row, update);
    }

    private void updateRow(Value[] row, StatementUpdate update) throws WCDBException {
        Handle handle = getHandle();
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(update);
            preparedStatement.bindRow(row);
            preparedStatement.step();
            preparedStatement.finalize();
        } finally {
            if(autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
    }

    public void deleteValue(String tableName) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName);
        deleteValue(delete);
    }

    public void deleteValue(String tableName, Expression condition) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition);
        deleteValue(delete);
    }

    public void deleteValue(String tableName, Expression condition, OrderingTerm orderingTerm, long limit) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition).orderBy(orderingTerm).limit(limit);
        deleteValue(delete);
    }

    public void deleteValue(String tableName, Expression condition, OrderingTerm orderingTerm, long limit, long offset) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition).orderBy(orderingTerm).limit(limit).offset(offset);
        deleteValue(delete);
    }

    public void deleteValue(String tableName, OrderingTerm orderingTerm, long limit) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).orderBy(orderingTerm).limit(limit);
        deleteValue(delete);
    }

    public void deleteValue(String tableName, OrderingTerm orderingTerm, long limit, long offset) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).orderBy(orderingTerm).limit(limit).offset(offset);
        deleteValue(delete);
    }

    private void deleteValue(StatementDelete delete) throws WCDBException {
        Handle handle = getHandle();
        try {
            handle.execute(delete);
        } finally {
            if(autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
    }

    public Value getValue(Field field) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)));
    }

    public Value getValue(Field field, Expression condition) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).where(condition));
    }

    public Value getValue(Field field, Expression condition, OrderingTerm order) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).where(condition).orderBy(order));
    }

    public Value getValue(Field field, Expression condition, OrderingTerm order, long offset) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).where(condition).orderBy(order).limit(1).offset(offset));
    }

    public Value getValue(Field field, OrderingTerm order) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).orderBy(order));
    }

    public Value getValue(Field field, OrderingTerm order, long offset) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).orderBy(order).limit(1).offset(offset));
    }

    public Value getValue(ResultColumnConvertible column, String tableName) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName));
    }

    public Value getValue(ResultColumnConvertible column, String tableName, Expression condition) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    public Value getValue(ResultColumnConvertible column, String tableName, Expression condition, OrderingTerm order) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    public Value getValue(ResultColumnConvertible column, String tableName, Expression condition, OrderingTerm order, long offset) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(1).offset(offset));
    }

    public Value getValue(ResultColumnConvertible column, String tableName, OrderingTerm order) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    public Value getValue(ResultColumnConvertible column, String tableName, OrderingTerm order, long offset) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(1).offset(offset));
    }

    public ArrayList<Value> getOneColumn(Field field) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)));
    }

    public ArrayList<Value> getOneColumn(Field field, Expression condition) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).where(condition));
    }

    public ArrayList<Value> getOneColumn(Field field, Expression condition, OrderingTerm order) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).where(condition).orderBy(order));
    }

    public ArrayList<Value> getOneColumn(Field field, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).where(condition).orderBy(order).limit(limit));
    }

    public ArrayList<Value> getOneColumn(Field field, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public ArrayList<Value> getOneColumn(Field field, OrderingTerm order) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).orderBy(order));
    }

    public ArrayList<Value> getOneColumn(Field field, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).orderBy(order).limit(limit));
    }

    public ArrayList<Value> getOneColumn(Field field, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(field).from(Field.getTableName(field)).orderBy(order).limit(limit).offset(offset));
    }

    public ArrayList<Value> getOneColumn(ResultColumnConvertible column, String tableName) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName));
    }

    public ArrayList<Value> getOneColumn(ResultColumnConvertible column, String tableName, Expression condition) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    public ArrayList<Value> getOneColumn(ResultColumnConvertible column, String tableName, Expression condition, OrderingTerm order) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    public ArrayList<Value> getOneColumn(ResultColumnConvertible column, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    public ArrayList<Value> getOneColumn(ResultColumnConvertible column, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public ArrayList<Value> getOneColumn(ResultColumnConvertible column, String tableName, OrderingTerm order) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    public ArrayList<Value> getOneColumn(ResultColumnConvertible column, String tableName, OrderingTerm order, long limit) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    public ArrayList<Value> getOneColumn(ResultColumnConvertible column, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    public Value[] getOneRow(Field[] fields) {
        return getOneRowFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)));
    }

    public Value[] getOneRow(Field[] fields, Expression condition) {
        return getOneRowFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).where(condition));
    }

    public Value[] getOneRow(Field[] fields, Expression condition, OrderingTerm orderingTerm) {
        return getOneRowFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).where(condition).orderBy(orderingTerm));
    }

    public Value[] getOneRow(Field[] fields, Expression condition, OrderingTerm orderingTerm, long offset) {
        return getOneRowFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).where(condition).orderBy(orderingTerm).limit(1).offset(offset));
    }

    public Value[] getOneRow(Field[] fields, OrderingTerm orderingTerm) {
        return getOneRowFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).orderBy(orderingTerm));
    }

    public Value[] getOneRow(Field[] fields, OrderingTerm orderingTerm, long offset) {
        return getOneRowFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).orderBy(orderingTerm).limit(1).offset(offset));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, String tableName) {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, String tableName, Expression condition) {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, String tableName, Expression condition, OrderingTerm orderingTerm) {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(orderingTerm));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, String tableName, Expression condition, OrderingTerm orderingTerm, long offset) {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(orderingTerm).limit(1).offset(offset));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, String tableName, OrderingTerm orderingTerm) {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(orderingTerm));
    }

    public Value[] getOneRow(ResultColumnConvertible[] columns, String tableName, OrderingTerm orderingTerm, long offset) {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(orderingTerm).limit(1).offset(offset));
    }

    public ArrayList<Value[]> getAllRows(Field[] fields) {
        return getAllRowsFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)));
    }

    public ArrayList<Value[]> getAllRows(Field[] fields, Expression condition) {
        return getAllRowsFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).where(condition));
    }

    public ArrayList<Value[]> getAllRows(Field[] fields, Expression condition, OrderingTerm order) {
        return getAllRowsFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).where(condition).orderBy(order));
    }

    public ArrayList<Value[]> getAllRows(Field[] fields, Expression condition, OrderingTerm order, long limit) {
        return getAllRowsFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).where(condition).orderBy(order).limit(limit));
    }

    public ArrayList<Value[]> getAllRows(Field[] fields, Expression condition, OrderingTerm order, long limit, long offset) {
        return getAllRowsFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public ArrayList<Value[]> getAllRows(Field[] fields, OrderingTerm order) {
        return getAllRowsFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).orderBy(order));
    }

    public ArrayList<Value[]> getAllRows(Field[] fields, OrderingTerm order, long limit) {
        return getAllRowsFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).orderBy(order).limit(limit));
    }

    public ArrayList<Value[]> getAllRows(Field[] fields, OrderingTerm order, long limit, long offset) {
        return getAllRowsFromStatement(new StatementSelect().select(fields).from(Field.getTableName(fields)).orderBy(order).limit(limit).offset(offset));
    }

    public ArrayList<Value[]> getAllRows(ResultColumnConvertible[] columns, String tableName) {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName));
    }

    public ArrayList<Value[]> getAllRows(ResultColumnConvertible[] columns, String tableName, Expression condition) {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition));
    }

    public ArrayList<Value[]> getAllRows(ResultColumnConvertible[] columns, String tableName, Expression condition, OrderingTerm order) {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order));
    }

    public ArrayList<Value[]> getAllRows(ResultColumnConvertible[] columns, String tableName, Expression condition, OrderingTerm order, long limit) {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    public ArrayList<Value[]> getAllRows(ResultColumnConvertible[] columns, String tableName, Expression condition, OrderingTerm order, long limit, long offset) {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    public ArrayList<Value[]> getAllRows(ResultColumnConvertible[] columns, String tableName, OrderingTerm order) {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order));
    }

    public ArrayList<Value[]> getAllRows(ResultColumnConvertible[] columns, String tableName, OrderingTerm order, long limit) {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order).limit(limit));
    }

    public ArrayList<Value[]> getAllRows(ResultColumnConvertible[] columns, String tableName, OrderingTerm order, long limit, long offset) {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    public Value getValueFromStatement(Statement statement) throws WCDBException {
        Handle handle = getHandle();;
        Value ret = null;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            preparedStatement.step();
            if(!preparedStatement.isDone()) {
                ret = preparedStatement.getValue(0);
            }
            preparedStatement.finalize();
        } finally {
            if(autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    public Value[] getOneRowFromStatement(Statement statement) throws WCDBException {
        Handle handle = getHandle();
        Value[] ret = null;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            preparedStatement.step();
            if(!preparedStatement.isDone()) {
                ret = preparedStatement.getOneRow();
            }
            preparedStatement.finalize();
        } finally {
            if(autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    public ArrayList<Value> getOneColumnFromStatement(Statement statement) throws WCDBException {
        Handle handle = getHandle();;
        ArrayList<Value> ret = null;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumn();
            preparedStatement.finalize();
        } finally {
            if(autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    public ArrayList<Value[]> getAllRowsFromStatement(Statement statement) throws WCDBException {
        Handle handle = getHandle();;
        ArrayList<Value[]> ret = null;
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getMultiRows();
            preparedStatement.finalize();
        } finally {
            if(autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    public void execute(Statement statement) throws WCDBException {
    }
}

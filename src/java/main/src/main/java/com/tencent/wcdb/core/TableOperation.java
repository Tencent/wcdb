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

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.util.Collection;
import java.util.Collections;
import java.util.List;

public class TableOperation {
    String tableName = null;
    Database database = null;

    /**
     * Get table name.
     */
    @NotNull
    public String getTableName() {
        return tableName;
    }

    /**
     * Get related database.
     */
    @NotNull
    public Database getDatabase() {
        return database;
    }

    /**
     * Execute inserting with one row of values.
     *
     * @param row     One row of value.
     * @param columns Corresponding column of values.
     * @throws WCDBException if any error occurs.
     */
    public void insertRow(@NotNull Value[] row, @Nullable Column[] columns) throws WCDBException {
        insertRows(Collections.singleton(row), columns);
    }

    /**
     * Execute inserting with multi rows of values.
     * It will run embedded transaction while rows.size()>1.
     * The embedded transaction means that it will run a transaction if it's not in other transaction,
     * otherwise it will be executed within the existing transaction.
     *
     * @param rows    Multi rows of value.
     * @param columns Corresponding column of values.
     * @throws WCDBException if any error occurs.
     */
    public void insertRows(@NotNull Collection<Value[]> rows, @Nullable Column[] columns) throws WCDBException {
        insertRows(rows, columns, ConflictAction.None);
    }

    /**
     * Execute inserting with one row of values.
     * It will replace the original row while they have same primary key or row id.
     *
     * @param row     One row of value.
     * @param columns Corresponding column of values.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceRow(@NotNull Value[] row, @Nullable Column[] columns) throws WCDBException {
        insertOrReplaceRows(Collections.singleton(row), columns);
    }

    /**
     * Execute inserting with multi rows of values.
     * It will replace the original row while they have same primary key or row id.
     * It will run embedded transaction while rows.size()>1.
     * The embedded transaction means that it will run a transaction if it's not in other transaction,
     * otherwise it will be executed within the existing transaction.
     *
     * @param rows    Multi rows of value.
     * @param columns Corresponding column of values.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceRows(@NotNull Collection<Value[]> rows, @Nullable Column[] columns) throws WCDBException {
        insertRows(rows, columns, ConflictAction.Replace);
    }

    /**
     * Execute inserting with one row of values.
     * It will ignore the row while there already exists the same primary key or row id in current table.
     *
     * @param row     One row of value.
     * @param columns Corresponding column of values.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreRow(@NotNull Value[] row, @Nullable Column[] columns) throws WCDBException {
        insertOrIgnoreRows(Collections.singleton(row), columns);
    }

    /**
     * Execute inserting with multi rows of values.
     * It will ignore the row while there already exists the same primary key or row id in current table.
     * It will run embedded transaction while rows.size()>1.
     * The embedded transaction means that it will run a transaction if it's not in other transaction,
     * otherwise it will be executed within the existing transaction.
     *
     * @param rows    Multi rows of value.
     * @param columns Corresponding column of values.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreRows(@NotNull Collection<Value[]> rows, @Nullable Column[] columns) throws WCDBException {
        insertRows(rows, columns, ConflictAction.Ignore);
    }

    private void insertRows(final Collection<Value[]> rows, Column[] columns, ConflictAction action) throws WCDBException {
        final StatementInsert insert = new StatementInsert().insertInto(tableName).columns(columns).valuesWithBindParameters(columns.length);
        if (action == ConflictAction.Replace) {
            insert.orReplace();
        } else if (action == ConflictAction.Ignore) {
            insert.orIgnore();
        }
        Handle handle = database.getHandle(true);
        try {
            if (rows.size() > 1) {
                handle.runTransaction(new Transaction() {
                    @Override
                    public boolean insideTransaction(@NotNull Handle handle) throws WCDBException {
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

    private void insertRows(Collection<Value[]> rows, StatementInsert insert, Handle handle) throws WCDBException {
        PreparedStatement preparedStatement = handle.preparedWithMainStatement(insert);
        for (Value[] row : rows) {
            preparedStatement.reset();
            preparedStatement.bindRow(row);
            preparedStatement.step();
        }
        preparedStatement.finalizeStatement();
    }

    public void updateValue(int value, @NotNull Column column) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column});
    }

    public void updateValue(int value, @NotNull Column column, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition);
    }

    public void updateValue(int value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(int value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(int value, @NotNull Column column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit);
    }

    public void updateValue(int value, @NotNull Column column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit, offset);
    }

    public void updateValue(long value, @NotNull Column column) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column});
    }

    public void updateValue(long value, @NotNull Column column, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition);
    }

    public void updateValue(long value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(long value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(long value, @NotNull Column column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit);
    }

    public void updateValue(long value, @NotNull Column column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit, offset);
    }

    public void updateValue(float value, @NotNull Column column) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column});
    }

    public void updateValue(float value, @NotNull Column column, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition);
    }

    public void updateValue(float value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(float value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(float value, @NotNull Column column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit);
    }

    public void updateValue(float value, @NotNull Column column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit, offset);
    }

    public void updateValue(double value, @NotNull Column column) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column});
    }

    public void updateValue(double value, @NotNull Column column, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition);
    }

    public void updateValue(double value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(double value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(double value, @NotNull Column column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit);
    }

    public void updateValue(double value, @NotNull Column column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit, offset);
    }

    public void updateValue(@Nullable String value, @NotNull Column column) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column});
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition);
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit);
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit, offset);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column});
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, order, limit, offset);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column});
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, condition);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, condition, order, limit);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, condition, order, limit, offset);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, order, limit);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, order, limit, offset);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns);
        updateRow(row, update);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @Nullable Expression condition) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition);
        updateRow(row, update);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition).orderBy(order).limit(limit);
        updateRow(row, update);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition).orderBy(order).limit(limit).offset(offset);
        updateRow(row, update);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @Nullable OrderingTerm order, int limit) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).orderBy(order).limit(limit);
        updateRow(row, update);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).orderBy(order).limit(limit).offset(offset);
        updateRow(row, update);
    }

    private void updateRow(@NotNull Value[] row, StatementUpdate update) throws WCDBException {
        Handle handle = database.getHandle(true);
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(update);
            preparedStatement.bindRow(row);
            preparedStatement.step();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
    }

    public void deleteValue() throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName);
        deleteValue(delete);
    }

    public void deleteValue(@Nullable Expression condition) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition);
        deleteValue(delete);
    }

    public void deleteValue(@Nullable Expression condition, @Nullable OrderingTerm orderingTerm, int limit) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition).orderBy(orderingTerm).limit(limit);
        deleteValue(delete);
    }

    public void deleteValue(@Nullable Expression condition, @Nullable OrderingTerm orderingTerm, int limit, int offset) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition).orderBy(orderingTerm).limit(limit).offset(offset);
        deleteValue(delete);
    }

    public void deleteValue(@Nullable OrderingTerm orderingTerm, int limit) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).orderBy(orderingTerm).limit(limit);
        deleteValue(delete);
    }

    public void deleteValue(@Nullable OrderingTerm orderingTerm, int limit, int offset) throws WCDBException {
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

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @Nullable Expression condition) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int offset) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(1).offset(offset));
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int offset) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(1).offset(offset));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @Nullable Expression condition) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @Nullable Expression condition) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @Nullable Expression condition, @Nullable OrderingTerm orderingTerm) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(orderingTerm));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @Nullable Expression condition, @Nullable OrderingTerm orderingTerm, int offset) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(orderingTerm).limit(1).offset(offset));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @Nullable OrderingTerm orderingTerm) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(orderingTerm));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @Nullable OrderingTerm orderingTerm, int offset) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(orderingTerm).limit(1).offset(offset));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @Nullable Expression condition) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @Nullable Expression condition) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @Nullable Expression condition) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @Nullable Expression condition) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @Nullable Expression condition) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @Nullable Expression condition) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @Nullable Expression condition) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @Nullable OrderingTerm order) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @Nullable
    public Value getValueFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        Value ret = null;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            preparedStatement.step();
            if (!preparedStatement.isDone()) {
                ret = preparedStatement.getValue(0);
            }
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
        return ret;
    }

    @Nullable
    public Value[] getOneRowFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        Value[] ret = null;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            preparedStatement.step();
            if (!preparedStatement.isDone()) {
                ret = preparedStatement.getOneRow();
            }
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
        return ret;
    }

    @NotNull
    public List<Value> getOneColumnFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Value> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumn();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
        return ret;
    }

    @NotNull
    public List<Integer> getOneColumnIntFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Integer> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnInt();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
        return ret;
    }

    @NotNull
    public List<Long> getOneColumnLongFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Long> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnLong();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
        return ret;
    }

    @NotNull
    public List<Float> getOneColumnFloatFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Float> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnFloat();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
        return ret;
    }

    @NotNull
    public List<Double> getOneColumnDoubleFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Double> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnDouble();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
        return ret;
    }

    @NotNull
    public List<String> getOneColumnStringFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<String> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnString();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
        return ret;
    }

    @NotNull
    public List<byte[]> getOneColumnBLOBFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<byte[]> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnBLOB();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
        return ret;
    }

    @NotNull
    public List<Value[]> getAllRowsFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = database.getHandle(false);
        List<Value[]> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getMultiRows();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            handle.invalidate();
        }
        return ret;
    }
}
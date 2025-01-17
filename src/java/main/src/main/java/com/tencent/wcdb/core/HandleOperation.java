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

public abstract class HandleOperation extends CppObject {
    abstract Handle getHandle(boolean writeHint);

    abstract boolean autoInvalidateHandle();

    /**
     * Execute inserting with one row of values.
     *
     * @param row       One row of value.
     * @param columns   Corresponding column of values.
     * @param tableName The table to insert value.
     * @throws WCDBException if any error occurs.
     */
    public void insertRow(@NotNull Value[] row, @Nullable Column[] columns, @NotNull String tableName) throws WCDBException {
        insertRows(Collections.singleton(row), columns, tableName);
    }

    /**
     * Execute inserting with multi rows of values.
     * It will run embedded transaction while rows.size()>1.
     * The embedded transaction means that it will run a transaction if it's not in other transaction,
     * otherwise it will be executed within the existing transaction.
     *
     * @param rows      Multi rows of value.
     * @param columns   Corresponding column of values.
     * @param tableName The table to insert value.
     * @throws WCDBException if any error occurs.
     */
    public void insertRows(@NotNull Collection<Value[]> rows, @Nullable Column[] columns, @NotNull String tableName) throws WCDBException {
        insertRows(rows, columns, tableName, ConflictAction.None);
    }

    /**
     * Execute inserting with one row of values.
     * It will replace the original row while they have same primary key or row id.
     *
     * @param row       One row of value.
     * @param columns   Corresponding column of values.
     * @param tableName The table to insert value.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceRow(@NotNull Value[] row, @Nullable Column[] columns, @NotNull String tableName) throws WCDBException {
        insertOrReplaceRows(Collections.singleton(row), columns, tableName);
    }

    /**
     * Execute inserting with multi rows of values.
     * It will replace the original row while they have same primary key or row id.
     * It will run embedded transaction while rows.size()>1.
     * The embedded transaction means that it will run a transaction if it's not in other transaction,
     * otherwise it will be executed within the existing transaction.
     *
     * @param rows      Multi rows of value.
     * @param columns   Corresponding column of values.
     * @param tableName The table to insert value.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceRows(@NotNull Collection<Value[]> rows, @Nullable Column[] columns, @NotNull String tableName) throws WCDBException {
        insertRows(rows, columns, tableName, ConflictAction.Replace);
    }

    /**
     * Execute inserting with one row of values.
     * It will ignore the row while there already exists the same primary key or row id in current table.
     *
     * @param row       One row of value.
     * @param columns   Corresponding column of values.
     * @param tableName The table to insert value.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreRow(@NotNull Value[] row, @Nullable Column[] columns, @NotNull String tableName) throws WCDBException {
        insertOrIgnoreRows(Collections.singleton(row), columns, tableName);
    }

    /**
     * Execute inserting with multi rows of values.
     * It will ignore the row while there already exists the same primary key or row id in current table.
     * It will run embedded transaction while rows.size()>1.
     * The embedded transaction means that it will run a transaction if it's not in other transaction,
     * otherwise it will be executed within the existing transaction.
     *
     * @param rows      Multi rows of value.
     * @param columns   Corresponding column of values.
     * @param tableName The table to insert value.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreRows(@NotNull Collection<Value[]> rows, @Nullable Column[] columns, @NotNull String tableName) throws WCDBException {
        insertRows(rows, columns, tableName, ConflictAction.Ignore);
    }

    private void insertRows(final Collection<Value[]> rows, Column[] columns, String tableName, ConflictAction action) throws WCDBException {
        final StatementInsert insert = new StatementInsert().insertInto(tableName).columns(columns).valuesWithBindParameters(columns.length);
        if (action == ConflictAction.Replace) {
            insert.orReplace();
        } else if (action == ConflictAction.Ignore) {
            insert.orIgnore();
        }
        Handle handle = getHandle(true);
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
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
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

    public void updateValue(int value, @NotNull Column column, @NotNull String tableName) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName);
    }

    public void updateValue(int value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition);
    }

    public void updateValue(int value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit);
    }

    public void updateValue(int value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit, offset);
    }

    public void updateValue(int value, @NotNull Column column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit);
    }

    public void updateValue(int value, @NotNull Column column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit, offset);
    }

    public void updateValue(long value, @NotNull Column column, @NotNull String tableName) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName);
    }

    public void updateValue(double value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition);
    }

    public void updateValue(double value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit);
    }

    public void updateValue(double value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit, offset);
    }

    public void updateValue(double value, @NotNull Column column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit);
    }

    public void updateValue(double value, @NotNull Column column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit, offset);
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @NotNull String tableName) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName);
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition);
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit);
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit, offset);
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit);
    }

    public void updateValue(@Nullable String value, @NotNull Column column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit, offset);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @NotNull String tableName) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, condition, order, limit, offset);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit);
    }

    public void updateValue(@Nullable byte[] value, @NotNull Column column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{new Value(value)}, new Column[]{column}, tableName, order, limit, offset);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @NotNull String tableName) throws WCDBException {
        if (value == null) {
            value = new Value();
        }
        updateRow(new Value[]{value}, new Column[]{column}, tableName);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        if (value == null) {
            value = new Value();
        }
        updateRow(new Value[]{value}, new Column[]{column}, tableName, condition);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        if (value == null) {
            value = new Value();
        }
        updateRow(new Value[]{value}, new Column[]{column}, tableName, condition, order, limit);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, tableName, condition, order, limit, offset);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        if (value == null) {
            value = new Value();
        }
        updateRow(new Value[]{value}, new Column[]{column}, tableName, order, limit);
    }

    public void updateValue(@Nullable Value value, @NotNull Column column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        updateRow(new Value[]{value}, new Column[]{column}, tableName, order, limit, offset);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @NotNull String tableName) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns);
        updateRow(row, update);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition);
        updateRow(row, update);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition).orderBy(order).limit(limit);
        updateRow(row, update);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).where(condition).orderBy(order).limit(limit).offset(offset);
        updateRow(row, update);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).orderBy(order).limit(limit);
        updateRow(row, update);
    }

    public void updateRow(@NotNull Value[] row, @NotNull Column[] columns, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        StatementUpdate update = new StatementUpdate().update(tableName).setColumnsToBindParameters(columns).orderBy(order).limit(limit).offset(offset);
        updateRow(row, update);
    }

    private void updateRow(@NotNull Value[] row, StatementUpdate update) throws WCDBException {
        Handle handle = getHandle(true);
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(update);
            preparedStatement.bindRow(row);
            preparedStatement.step();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
    }

    public void deleteValue(@NotNull String tableName) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName);
        deleteValue(delete);
    }

    public void deleteValue(@NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition);
        deleteValue(delete);
    }

    public void deleteValue(@NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm orderingTerm, int limit) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition).orderBy(orderingTerm).limit(limit);
        deleteValue(delete);
    }

    public void deleteValue(@NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm orderingTerm, int limit, int offset) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).where(condition).orderBy(orderingTerm).limit(limit).offset(offset);
        deleteValue(delete);
    }

    public void deleteValue(@NotNull String tableName, @Nullable OrderingTerm orderingTerm, int limit) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).orderBy(orderingTerm).limit(limit);
        deleteValue(delete);
    }

    public void deleteValue(@NotNull String tableName, @Nullable OrderingTerm orderingTerm, int limit, int offset) throws WCDBException {
        StatementDelete delete = new StatementDelete().deleteFrom(tableName).orderBy(orderingTerm).limit(limit).offset(offset);
        deleteValue(delete);
    }

    private void deleteValue(StatementDelete delete) throws WCDBException {
        Handle handle = getHandle(true);
        try {
            handle.execute(delete);
        } finally {
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @NotNull String tableName) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int offset) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(1).offset(offset));
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @Nullable
    public Value getValue(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int offset) throws WCDBException {
        return getValueFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(1).offset(offset));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @NotNull String tableName) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Value> getOneColumn(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm orderingTerm) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(orderingTerm));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm orderingTerm, int offset) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(orderingTerm).limit(1).offset(offset));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable OrderingTerm orderingTerm) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(orderingTerm));
    }

    @Nullable
    public Value[] getOneRow(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable OrderingTerm orderingTerm, int offset) throws WCDBException {
        return getOneRowFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(orderingTerm).limit(1).offset(offset));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @NotNull String tableName) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Integer> getOneColumnInt(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnIntFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @NotNull String tableName) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Long> getOneColumnLong(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnLongFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @NotNull String tableName) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Float> getOneColumnFloat(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnFloatFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @NotNull String tableName) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Double> getOneColumnDouble(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnDoubleFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @NotNull String tableName) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<String> getOneColumnString(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnStringFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @NotNull String tableName) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<byte[]> getOneColumnBLOB(@NotNull ResultColumnConvertible column, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getOneColumnBLOBFromStatement(new StatementSelect().select(column).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable Expression condition) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable OrderingTerm order) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable OrderingTerm order, int limit) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order).limit(limit));
    }

    @NotNull
    public List<Value[]> getAllRows(@NotNull ResultColumnConvertible[] columns, @NotNull String tableName, @Nullable OrderingTerm order, int limit, int offset) throws WCDBException {
        return getAllRowsFromStatement(new StatementSelect().select(columns).from(tableName).orderBy(order).limit(limit).offset(offset));
    }

    @Nullable
    public Value getValueFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(false);
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
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @Nullable
    public Value getValueFromSQL(@NotNull String sql) throws WCDBException {
        Handle handle = getHandle(false);
        Value ret = null;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(sql);
            preparedStatement.step();
            if (!preparedStatement.isDone()) {
                ret = preparedStatement.getValue(0);
            }
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @Nullable
    public Value[] getOneRowFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(false);
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
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @Nullable
    public Value[] getOneRowFromSQL(@NotNull String sql) throws WCDBException {
        Handle handle = getHandle(false);
        Value[] ret = null;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(sql);
            preparedStatement.step();
            if (!preparedStatement.isDone()) {
                ret = preparedStatement.getOneRow();
            }
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @NotNull
    public List<Value> getOneColumnFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(false);
        List<Value> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumn();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @NotNull
    public List<Integer> getOneColumnIntFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(false);
        List<Integer> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnInt();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @NotNull
    public List<Long> getOneColumnLongFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(false);
        List<Long> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnLong();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @NotNull
    public List<Float> getOneColumnFloatFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(false);
        List<Float> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnFloat();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @NotNull
    public List<Double> getOneColumnDoubleFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(false);
        List<Double> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnDouble();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @NotNull
    public List<String> getOneColumnStringFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(false);
        List<String> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnString();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @NotNull
    public List<byte[]> getOneColumnBLOBFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(false);
        List<byte[]> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getOneColumnBLOB();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @NotNull
    public List<Value> getOneColumnFromSQL(@NotNull String sql) throws WCDBException {
        Handle handle = getHandle(false);
        List<Value> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(sql);
            ret = preparedStatement.getOneColumn();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @NotNull
    public List<Value[]> getAllRowsFromStatement(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(false);
        List<Value[]> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(statement);
            ret = preparedStatement.getMultiRows();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    @NotNull
    public List<Value[]> getAllRowsFromSQL(@NotNull String sql) throws WCDBException {
        Handle handle = getHandle(false);
        List<Value[]> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = handle.preparedWithMainStatement(sql);
            ret = preparedStatement.getMultiRows();
        } finally {
            if (preparedStatement != null) {
                preparedStatement.finalizeStatement();
            }
            if (autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return ret;
    }

    /**
     * Execute a statement directly.
     *
     * @param statement The statement to execute.
     * @throws WCDBException if any error occurs.
     */
    public void execute(@NotNull Statement statement) throws WCDBException {
        Handle handle = getHandle(statement.isWriteStatement());
        WCDBException exception = null;
        if (!Handle.execute(handle.getCppHandle(), CppObject.get(statement))) {
            exception = handle.createException();
        }
        if (autoInvalidateHandle()) {
            handle.invalidate();
        }
        if (exception != null) {
            throw exception;
        }
    }

    /**
     * Execute a sql string directly.
     * Note that you should no execute sql string on a migrating or compressing table.
     *
     * @param sql The sql string to execute.
     * @throws WCDBException if any error occurs.
     */
    public void execute(@NotNull String sql) throws WCDBException {
        Handle handle = getHandle(false);
        WCDBException exception = null;
        if (!Handle.executeSQL(handle.getCppHandle(), sql)) {
            exception = handle.createException();
        }
        if (autoInvalidateHandle()) {
            handle.invalidate();
        }
        if (exception != null) {
            throw exception;
        }
    }

    /**
     * Check whether the current database has begun a transaction in the current thread with {@code sqlite3_get_autocommit}.
     *
     * @return True if current database has begun a transaction in the current thread.
     * @throws WCDBException if any error occurs.
     */
    public boolean isInTransaction() throws WCDBException {
        Handle handle = null;
        boolean res;
        try {
            handle = getHandle(false);
            res = Handle.isInTransaction(handle.getCppHandle());
        } finally {
            if (handle != null && autoInvalidateHandle()) {
                handle.invalidate();
            }
        }
        return res;
    }

    /**
     * Begin a transaction.
     * Separate interface of {@link HandleOperation#runTransaction(Transaction)}
     * You should call {@link HandleOperation#beginTransaction()}, {@link HandleOperation#commitTransaction()}, {@link HandleOperation#rollbackTransaction()} and all other operations in same thread.
     *
     * @throws WCDBException if any error occurs.
     */
    public void beginTransaction() throws WCDBException {
        Handle handle = getHandle(true);
        WCDBException exception = null;
        if (!Handle.beginTransaction(handle.getCppHandle())) {
            exception = handle.createException();
        }
        if (autoInvalidateHandle()) {
            handle.invalidate();
        }
        if (exception != null) {
            throw exception;
        }
    }

    /**
     * Commit current transaction.
     * Separate interface of {@link HandleOperation#runTransaction(Transaction)}
     * You should call {@link HandleOperation#beginTransaction()}, {@link HandleOperation#commitTransaction()}, {@link HandleOperation#rollbackTransaction()} and all other operations in same thread.
     *
     * @throws WCDBException if any error occurs.
     */
    public void commitTransaction() throws WCDBException {
        Handle handle = getHandle(true);
        WCDBException exception = null;
        if (!Handle.commitTransaction(handle.getCppHandle())) {
            exception = handle.createException();
        }
        if (autoInvalidateHandle()) {
            handle.invalidate();
        }
        if (exception != null) {
            throw exception;
        }
    }

    /**
     * Rollback current transaction.
     * Separate interface of {@link HandleOperation#runTransaction(Transaction)}
     * You should call {@link HandleOperation#beginTransaction()}, {@link HandleOperation#commitTransaction()}, {@link HandleOperation#rollbackTransaction()} and all other operations in same thread.
     *
     * @throws WCDBException if any error occurs.
     */
    public void rollbackTransaction() throws WCDBException {
        Handle handle = getHandle(true);
        Handle.rollbackTransaction(handle.getCppHandle());
        if (autoInvalidateHandle()) {
            handle.invalidate();
        }
    }

    /**
     * Run a transaction in a closure. Transaction supports nesting.
     *
     * @param transaction The operation inside transaction.
     * @throws WCDBException if any error occurs.
     */
    public void runTransaction(@NotNull Transaction transaction) throws WCDBException {
        Handle handle = getHandle(true);
        WCDBException exception = null;
        if (!handle.runTransaction(handle.getCppHandle(), transaction)) {
            exception = handle.createException();
        }
        if (autoInvalidateHandle()) {
            handle.invalidate();
        }
        if (exception != null) {
            throw exception;
        }
    }

    /**
     * Run a pausable transaction in a closure.
     * Firstly, WCDB will begin a transaction and call the block.
     * After the block is finished, WCDB will check whether the main thread is suspended due to the current transaction.
     * If not, it will call the block again; if it is, it will temporarily commit the current transaction.
     * Once database operations in main thread are finished, WCDB will rebegin a new transaction in the current thread and call the block.
     * This process will be repeated until the return value of the block is specified as true, or some error occurs during the transaction.
     * You can use pausable transaction to do some long term database operations, such as data cleaning or data migration, and avoid to block the main thread.
     * <pre>
     *     <code>
     *         database.runPausableTransaction(new PausableTransaction() {
     *             public boolean insideTransaction(Handle handle, boolean isNewTransaction) throws WCDBException {
     *                 if(isNewTransaction) {
     *                     // Do some initialization for new transaction.
     *                 }
     *                 // Perform a small amount of data processing.
     *                 if( Error occurs ) {
     *                     throws WCDBException;
     *                 }
     *                 if( All database operations are finished ) {
     *                     return true;
     *                 }
     *                 return false;
     *             }
     *         });
     *     </code>
     * </pre>
     *
     * @param transaction The operation inside transaction.
     * @throws WCDBException if any error occurs.
     */
    public void runPausableTransaction(@NotNull PausableTransaction transaction) throws WCDBException {
        Handle handle = getHandle(true);
        WCDBException exception = null;
        if (!handle.runPausableTransaction(handle.getCppHandle(), transaction)) {
            exception = handle.createException();
        }
        if (autoInvalidateHandle()) {
            handle.invalidate();
        }
        if (exception != null) {
            throw exception;
        }
    }
}

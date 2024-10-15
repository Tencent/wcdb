// Created by chenqiuwen on 2023/5/1.
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

package com.tencent.wcdb.chaincall;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.core.PreparedStatement;
import com.tencent.wcdb.core.Transaction;
import com.tencent.wcdb.orm.Field;
import com.tencent.wcdb.orm.TableBinding;
import com.tencent.wcdb.winq.StatementInsert;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.util.Collection;
import java.util.Collections;

public class Insert<T> extends ChainCall<StatementInsert> {
    private boolean hasConflictAction = false;
    private Field<T>[] fields = null;
    private Collection<T> values = null;
    private long lastInsertRowId = 0;

    public Insert(@NotNull Handle handle, boolean needChanges, boolean autoInvalidateHandle) {
        super(handle, needChanges, autoInvalidateHandle);
        statement = new StatementInsert();
    }

    /**
     * WINQ interface for SQL.
     *
     * @return this.
     */
    @NotNull
    public Insert<T> orReplace() {
        hasConflictAction = true;
        statement.orReplace();
        return this;
    }

    /**
     * WINQ interface for SQL.
     *
     * @return this.
     */
    @NotNull
    public Insert<T> orIgnore() {
        hasConflictAction = true;
        statement.orIgnore();
        return this;
    }

    /**
     * WINQ interface for SQL.
     *
     * @param table The name of the table to delete data from.
     * @return this
     */
    @NotNull
    public Insert<T> intoTable(@NotNull String table) {
        statement.insertInto(table);
        return this;
    }

    /**
     * WINQ interface for SQL.
     *
     * @param fields Do a partial insertion with the specific fields.
     * @return this.
     */
    @SafeVarargs
    @NotNull
    public final Insert<T> onFields(@NotNull Field<T>... fields) {
        this.fields = fields;
        statement.columns(fields).valuesWithBindParameters(fields.length);
        return this;
    }

    /**
     * Insert an object.
     *
     * @param object The object to be inserted into table.
     * @return this.
     */
    @NotNull
    public Insert<T> value(@Nullable T object) {
        values = Collections.singleton(object);
        return this;
    }

    /**
     * Insert a batch of objects
     *
     * @param objects Objects to be inserted into table.
     * @return this.
     */
    @NotNull
    public Insert<T> values(@NotNull Collection<T> objects) {
        values = objects;
        return this;
    }

    /**
     * Execute the insert statement.
     * Note that it will run embedded transaction while values.count>1.
     * The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     *
     * @return this.
     * @throws WCDBException if no error occurs.
     */
    @NotNull
    public Insert<T> execute() throws WCDBException {
        if (values.size() == 0) {
            return this;
        }
        assert fields != null && fields.length > 0;
        try {
            if (values.size() > 1) {
                handle.runTransaction(new Transaction() {
                    @Override
                    public boolean insideTransaction(@NotNull Handle handle) throws WCDBException {
                        realExecute();
                        return true;
                    }
                });
            } else {
                realExecute();
            }
        } finally {
            invalidateHandle();
        }
        return this;
    }

    /**
     * Get the rowid of the last inserted object.
     *
     * @return rowid.
     */
    public long getLastInsertRowId() {
        return lastInsertRowId;
    }

    private void realExecute() throws WCDBException {
        TableBinding<T> binding = Field.getBinding(fields);
        PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
        lastInsertRowId = 0;
        for (T object : values) {
            preparedStatement.reset();
            int index = 1;
            boolean isAutoIncrement = !hasConflictAction && binding.isAutoIncrement(object);
            for (Field<T> field : fields) {
                if (isAutoIncrement && field.isAutoIncrement()) {
                    preparedStatement.bindNull(index);
                } else {
                    binding.bindField(object, field, index, preparedStatement);
                }
                index++;
            }
            preparedStatement.step();
            if (isAutoIncrement) {
                binding.setLastInsertRowId(object, handle.getLastInsertedRowId());
            }
        }
        if (values.size() > 0) {
            lastInsertRowId = handle.getLastInsertedRowId();
        }
        updateChanges();
        preparedStatement.finalizeStatement();
    }

}

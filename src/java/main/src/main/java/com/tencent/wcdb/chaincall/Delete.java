// Created by chenqiuwen on 2023/5/2.
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
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.ExpressionConvertible;
import com.tencent.wcdb.winq.OrderingTerm;
import com.tencent.wcdb.winq.StatementDelete;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class Delete extends ChainCall<StatementDelete> {
    public Delete(@NotNull Handle handle, boolean needChanges, boolean autoInvalidateHandle) {
        super(handle, needChanges, autoInvalidateHandle);
        statement = new StatementDelete();
    }

    /**
     * WINQ interface for SQL.
     *
     * @param table The name of the table to delete data from.
     * @return this
     */
    @NotNull
    public Delete fromTable(@NotNull String table) {
        statement.deleteFrom(table);
        return this;
    }

    /**
     * WINQ interface for SQL.
     *
     * @param condition condition
     * @return this.
     */
    @NotNull
    public Delete where(@Nullable Expression condition) {
        statement.where(condition);
        return this;
    }

    /**
     * WINQ interface for SQL.
     *
     * @param orders order list
     * @return this.
     */
    @NotNull
    public Delete orderBy(@Nullable OrderingTerm... orders) {
        statement.orderBy(orders);
        return this;
    }

    /**
     * WINQ interface for SQL.
     *
     * @param count limit count.
     * @return this.
     */
    @NotNull
    public Delete limit(long count) {
        statement.limit(count);
        return this;
    }

    /**
     * WINQ interface for SQL.
     *
     * @param count limit expression.
     * @return this.
     */
    @NotNull
    public Delete limit(@NotNull ExpressionConvertible count) {
        statement.limit(count);
        return this;
    }

    /**
     * WINQ interface for SQL.
     *
     * @param offset offset number.
     * @return this.
     */
    @NotNull
    public Delete offset(long offset) {
        statement.offset(offset);
        return this;
    }

    /**
     * WINQ interface for SQL.
     *
     * @param offset offset expression.
     * @return this.
     */
    @NotNull
    public Delete offset(@Nullable ExpressionConvertible offset) {
        statement.offset(offset);
        return this;
    }

    /**
     * Execute the delete SQL.
     *
     * @return this.
     * @throws WCDBException if any error occurs.
     */
    @NotNull
    public Delete execute() throws WCDBException {
        try {
            handle.execute(statement);
        } finally {
            updateChanges();
            invalidateHandle();
        }
        return this;
    }
}

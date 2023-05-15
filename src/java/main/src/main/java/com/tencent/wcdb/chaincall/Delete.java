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

import java.time.chrono.MinguoDate;

public class Delete extends ChainCall<StatementDelete>{
    public Delete(Handle handle) {
        super(handle);
        statement = new StatementDelete();
    }

    public Delete fromTable(String table) {
        statement.deleteFrom(table);
        return this;
    }

    public Delete where(Expression condition) {
        statement.where(condition);
        return this;
    }

    public Delete orderBy(OrderingTerm order) {
        statement.orderBy(order);
        return this;
    }

    public Delete orderBy(OrderingTerm[] orders) {
        statement.orderBy(orders);
        return this;
    }

    public Delete limit(long count) {
        statement.limit(count);
        return this;
    }

    public Delete limit(ExpressionConvertible count) {
        statement.limit(count);
        return this;
    }

    public Delete offset(long offset) {
        statement.offset(offset);
        return this;
    }

    public Delete offset(ExpressionConvertible offset) {
        statement.offset(offset);
        return this;
    }

    public void execute() throws WCDBException {
        try {
            handle.execute(statement);
        } finally {
            updateChanges();
            invalidateHandle();
        }
    }
}

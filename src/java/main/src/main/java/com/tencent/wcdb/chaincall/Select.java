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
import com.tencent.wcdb.core.PreparedStatement;
import com.tencent.wcdb.orm.Field;
import com.tencent.wcdb.orm.TableBinding;
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.ExpressionConvertible;
import com.tencent.wcdb.winq.OrderingTerm;
import com.tencent.wcdb.winq.StatementSelect;

import java.util.ArrayList;

public class Select<T> extends ChainCall<StatementSelect> {
    private Field<T>[] fields = null;

    public Select(Handle handle) {
        super(handle);
        statement = new StatementSelect();
    }

    public Select<T> select(Field<T> field) {
        return select(new Field[]{field});
    }

    public Select<T> select(Field<T>[] fields) {
        this.fields = fields;
        statement.select(fields);
        return this;
    }

    public Select<T> where(Expression condition) {
        statement.where(condition);
        return this;
    }

    public Select<T> orderBy(OrderingTerm order) {
        statement.orderBy(order);
        return this;
    }

    public Select<T> orderBy(OrderingTerm[] orders) {
        statement.orderBy(orders);
        return this;
    }

    public Select<T> limit(long count) {
        statement.limit(count);
        return this;
    }

    public Select<T> limit(ExpressionConvertible count) {
        statement.limit(count);
        return this;
    }

    public Select<T> offset(long offset) {
        statement.offset(offset);
        return this;
    }

    public Select<T> offset(ExpressionConvertible offset) {
        statement.offset(offset);
        return this;
    }

    public Select<T> from(String tableName) {
        statement.from(tableName);
        return this;
    }

    public T firstObject() throws WCDBException {
        T ret = null;
        try {
            PreparedStatement preparedStatement = prepareStatement();
            preparedStatement.step();
            if(!preparedStatement.isDone()) {
                ret = preparedStatement.getOneObject(fields);
            }
            preparedStatement.finalizeStatement();
        } finally {
            invalidateHandle();
        }
        return ret;
    }

    public ArrayList<T> allObjects() throws WCDBException {
        ArrayList<T> ret = null;
        try {
            PreparedStatement preparedStatement = prepareStatement();
            ret = preparedStatement.getAllObjects(fields);
            preparedStatement.finalizeStatement();
        } finally {
            invalidateHandle();
        }
        return ret;
    }

    private PreparedStatement prepareStatement() throws WCDBException {
        return handle.preparedWithMainStatement(statement);
    }
}

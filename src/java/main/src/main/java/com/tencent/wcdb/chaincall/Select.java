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
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.ExpressionConvertible;
import com.tencent.wcdb.winq.OrderingTerm;
import com.tencent.wcdb.winq.StatementSelect;

import java.util.List;

public class Select<T> extends ChainCall<StatementSelect> {
    private Field<T>[] fields = null;

    public Select(Handle handle, boolean needChanges, boolean autoInvalidateHandle) {
        super(handle, needChanges, autoInvalidateHandle);
        statement = new StatementSelect();
    }

    /**
     * WINQ interface for SQL.
     * @param fields The column results to be selected.
     * @return
     */
    public Select<T> select(Field<T>... fields) {
        this.fields = fields;
        statement.select(fields);
        return this;
    }

    /**
     * WINQ interface for SQL.
     * @param condition condition.
     * @return this.
     */
    public Select<T> where(Expression condition) {
        statement.where(condition);
        return this;
    }

    /**
     * WINQ interface for SQL.
     * @param order order term.
     * @return this.
     */
    public Select<T> orderBy(OrderingTerm order) {
        statement.orderBy(order);
        return this;
    }

    /**
     * WINQ interface for SQL.
     * @param orders order list.
     * @return this.
     */
    public Select<T> orderBy(OrderingTerm... orders) {
        statement.orderBy(orders);
        return this;
    }

    /**
     * WINQ interface for SQL.
     * @param count limit count.
     * @return this.
     */
    public Select<T> limit(long count) {
        statement.limit(count);
        return this;
    }

    /**
     * WINQ interface for SQL.
     * @param count limit expression.
     * @return this.
     */
    public Select<T> limit(ExpressionConvertible count) {
        statement.limit(count);
        return this;
    }

    /**
     * WINQ interface for SQL.
     * @param offset offset number.
     * @return this.
     */
    public Select<T> offset(long offset) {
        statement.offset(offset);
        return this;
    }

    /**
     * WINQ interface for SQL.
     * @param offset offset expression.
     * @return this.
     */
    public Select<T> offset(ExpressionConvertible offset) {
        statement.offset(offset);
        return this;
    }

    /**
     * WINQ interface for SQL.
     * @param tableName The name of the table to query data from.
     * @return this.
     */
    public Select<T> from(String tableName) {
        statement.from(tableName);
        return this;
    }

    /**
     * Get first selected object.
     * @return a selected object.
     * @throws WCDBException if any error occurs.
     */
    public T firstObject() throws WCDBException {
        return firstObject(Field.getBindClass(fields));
    }

    /**
     * Get first selected object.
     * @param cls A derived class of java orm class.
     * @return The selected derived class object.
     * @throws WCDBException if any error occurs.
     */
    public <R extends T> R firstObject(Class<R> cls) throws WCDBException {
        R ret = null;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = prepareStatement();
            preparedStatement.step();
            if(!preparedStatement.isDone()) {
                ret = preparedStatement.getOneObject(fields, cls);
            }
        } finally {
            if(preparedStatement != null){
                preparedStatement.finalizeStatement();
            }
            invalidateHandle();
        }
        return ret;
    }

    /**
     * Get all selected objects.
     * @return A list of objects.
     * @throws WCDBException if any error occurs.
     */
    public List<T> allObjects() throws WCDBException {
        return allObjects(Field.getBindClass(fields));
    }

    /**
     * Get all selected objects.
     * @param cls A derived class of java orm class.
     * @return All selected derived class object.
     * @throws WCDBException if any error occurs.
     */
    public <R extends T> List<R> allObjects(Class<R> cls) throws WCDBException {
        List<R> ret;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = prepareStatement();
            ret = preparedStatement.getAllObjects(fields, cls);
        } finally {
            if(preparedStatement != null){
                preparedStatement.finalizeStatement();
            }
            invalidateHandle();
        }
        return ret;
    }

    private PreparedStatement prepareStatement() throws WCDBException {
        return handle.preparedWithMainStatement(statement);
    }
}

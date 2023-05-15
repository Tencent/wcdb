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

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.core.PreparedStatement;
import com.tencent.wcdb.orm.Field;
import com.tencent.wcdb.orm.TableBinding;
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.ExpressionConvertible;
import com.tencent.wcdb.winq.OrderingTerm;
import com.tencent.wcdb.winq.StatementUpdate;

public class Update<T> extends ChainCall<StatementUpdate> {
    private Field<T>[] fields = null;
    private T object = null;
    private Value[] row = null;
    private boolean hasTable = false;


    public Update(Handle handle) {
        super(handle);
        statement = new StatementUpdate();
    }

    public Update<T> table(String table) {
        hasTable = table != null && table.length() > 0;
        statement.update(table);
        return this;
    }

    public Update<T> set(Field<T> field) {
        return set(new Field[]{field});
    }

    public Update<T> set(Field<T>[] fields) {
        this.fields = fields;
        statement.setColumnsToBindParameters(fields);
        return this;
    }

    public Update<T> toObject(T object) {
        this.object = object;
        return this;
    }

    public Update<T> toValue(Value value) {
        if(value == null) {
            return toRow(new Value[]{new Value()});
        }
        return toRow(new Value[]{value});
    }

    public Update<T> toValue(long value) {
        return toRow(new Value[]{new Value(value)});
    }

    public Update<T> toValue(String value) {

        return toRow(new Value[]{new Value(value)});
    }

    public Update<T> toValue(byte[] value) {
        return toRow(new Value[]{new Value(value)});
    }

    public Update<T> toValue(double value) {
        return toRow(new Value[]{new Value(value)});
    }

    public Update<T> toRow(Value[] row) {
        this.row = row;
        return this;
    }

    public Update<T> where(Expression condition) {
        statement.where(condition);
        return this;
    }

    public Update<T> orderBy(OrderingTerm order) {
        statement.orderBy(order);
        return this;
    }

    public Update<T> orderBy(OrderingTerm[] orders) {
        statement.orderBy(orders);
        return this;
    }

    public Update<T> limit(long count) {
        statement.limit(count);
        return this;
    }

    public Update<T> limit(ExpressionConvertible count) {
        statement.limit(count);
        return this;
    }

    public Update<T> offset(long offset) {
        statement.offset(offset);
        return this;
    }

    public Update<T> offset(ExpressionConvertible offset) {
        statement.offset(offset);
        return this;
    }

    public void execute() throws WCDBException {
        assert object != null || row != null;
        TableBinding<T> binding = Field.getBinding(fields);
        if(!hasTable && binding != null) {
            statement.update(binding.bindingTableName());
        }
        try {
            PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
            if(object != null) {
                preparedStatement.bindObject(object, fields);
            }else if(row != null) {
                preparedStatement.bindRow(row);
            }
            preparedStatement.step();
            updateChanges();
            preparedStatement.finalize();
        } finally {
            invalidateHandle();
        }
    }
}

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

import java.util.Collection;
import java.util.Collections;

public class Insert<T> extends ChainCall<StatementInsert> {
    private boolean hasConflictAction = false;
    private Field<T>[] fields = null;
    private Collection<T> values = null;
    private long lastInsertRowId = 0;

    public Insert(Handle handle) {
        super(handle);
        statement = new StatementInsert();
    }

    public Insert<T> orReplace() {
        hasConflictAction = true;
        statement.orReplace();
        return this;
    }

    public Insert<T> orIgnore() {
        hasConflictAction = true;
        statement.orIgnore();
        return this;
    }

    public Insert<T> intoTable(String table) {
        statement.insertInto(table);
        return this;
    }

    public Insert<T> onFields(Field<T>... fields) {
        this.fields = fields;
        statement.columns(fields).valuesWithBindParameters(fields.length);
        return this;
    }

    public Insert<T> value(T object) {
        values = Collections.singleton(object);
        return this;
    }

    public Insert<T> values(Collection<T> objects) {
        values = objects;
        return this;
    }

    public Insert<T> execute() throws WCDBException {
        if(values == null || values.size() == 0) {
            return this;
        }
        assert fields != null && fields.length > 0;
        try {
            if (values.size() > 1) {
                handle.runTransaction(new Transaction() {
                    @Override
                    public boolean insideTransaction(Handle handle) throws WCDBException {
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

    public long getLastInsertRowId() {
        return lastInsertRowId;
    }

    private void realExecute() throws WCDBException {
        TableBinding<T> binding = Field.getBinding(fields);
        PreparedStatement preparedStatement = handle.preparedWithMainStatement(statement);
        lastInsertRowId = 0;
        if(binding != null) {
            for(T object : values) {
                preparedStatement.reset();
                int index = 1;
                boolean isAutoIncrement = !hasConflictAction && binding.isAutoIncrement(object);
                for (Field<T> field : fields) {
                    if(isAutoIncrement && field.isAutoIncrement()) {
                        preparedStatement.bindNull(index);
                    }else {
                        binding.bindField(object, field, index, preparedStatement);
                    }
                    index++;
                }
                preparedStatement.step();
                if(isAutoIncrement) {
                    binding.setLastInsertRowId(object, handle.getLastInsertedRowId());
                }
            }
            if(values.size() > 0) {
                lastInsertRowId = handle.getLastInsertedRowId();
            }
        }
        updateChanges();
        preparedStatement.finalizeStatement();
    }

}

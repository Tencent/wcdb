package com.tencent.wcdb.core;// Created by chenqiuwen on 2023/5/2.
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

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.chaincall.Delete;
import com.tencent.wcdb.chaincall.Insert;
import com.tencent.wcdb.chaincall.Select;
import com.tencent.wcdb.chaincall.Update;
import com.tencent.wcdb.orm.Field;
import com.tencent.wcdb.orm.TableBinding;
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.ExpressionConvertible;
import com.tencent.wcdb.winq.OrderingTerm;
import com.tencent.wcdb.winq.StatementDropTable;

import java.util.ArrayList;
import java.util.concurrent.locks.Condition;

public abstract class HandleORMOperation extends HandleOperation{
    abstract Database getDatabase();

    public <T> void createTable(String tableName, TableBinding<T> binding) throws WCDBException {
        Handle handle = getHandle();
        try {
            if(!binding.baseBinding().createTable(tableName, handle)) {
                throw handle.createException();
            }
        } finally {
            if(autoInvalidateHandle() && handle != null) {
                handle.invalidate();
            }
        }
    }

    public <T> void createVirtualTable(String tableName, TableBinding<T> binding) throws WCDBException {
        Handle handle = getHandle();
        try {
            if(!binding.baseBinding().createVirtualTable(tableName, handle)) {
                throw handle.createException();
            }
        } finally {
            if(autoInvalidateHandle() && handle != null) {
                handle.invalidate();
            }
        }
    }

    public boolean tableExist(String tableName) throws WCDBException {
        Handle handle = getHandle();
        int ret = handle.tableExist(handle.cppObj, tableName);
        WCDBException exception = null;
        if(ret > 1) {
            exception = handle.createException();
        }
        if(autoInvalidateHandle()) {
            handle.invalidate();
        }
        if(exception != null) {
            throw exception;
        }
        return ret == 1;
    }

    public <T> Table<T> getTable(String tableName, TableBinding<T> binding) {
        return new Table<T>(tableName, binding, getDatabase());
    }

    public void dropTable(String tableName) throws WCDBException {
        execute(new StatementDropTable().dropTable(tableName));
    }

    public <T> Insert<T> prepareInsert() throws WCDBException {
        Insert<T> insert = new Insert(getHandle());
        insert.autoInvalidateHandle = autoInvalidateHandle();
        insert.needChanges = false;
        return insert;
    }

    public <T> Update<T> prepareUpdate() throws WCDBException {
        Update<T> update = new Update(getHandle());
        update.autoInvalidateHandle = autoInvalidateHandle();
        update.needChanges = false;
        return update;
    }

    public <T> Select<T> prepareSelect() throws WCDBException {
        Select<T> select = new Select(getHandle());
        select.autoInvalidateHandle = autoInvalidateHandle();
        select.needChanges = false;
        return select;
    }

    public Delete prepareDelete() throws WCDBException {
        Delete delete = new Delete(getHandle());
        delete.autoInvalidateHandle = autoInvalidateHandle();
        delete.needChanges = false;
        return delete;
    }

    public <T> void insertObject(T object, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().intoTable(tableName).value(object).onFields(fields).execute();
    }

    public <T> void insertOrReplaceObject(T object, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().orReplace().intoTable(tableName).value(object).onFields(fields).execute();
    }

    public <T> void insertOrIgnoreObject(T object, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().orIgnore().intoTable(tableName).value(object).onFields(fields).execute();
    }

    public <T> void insertObjects(T[] objects, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().intoTable(tableName).values(objects).onFields(fields).execute();
    }

    public <T> void insertOrReplaceObjects(T[] objects, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().orReplace().intoTable(tableName).values(objects).onFields(fields).execute();
    }

    public <T> void insertOrIgnoreObjects(T[] objects, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().orIgnore().intoTable(tableName).values(objects).onFields(fields).execute();
    }

    public void deleteObjects(String tableName) throws WCDBException {
        prepareDelete().fromTable(tableName).execute();
    }

    public void deleteObjects(String tableName, Expression condition) throws WCDBException {
        prepareDelete().fromTable(tableName).where(condition).execute();
    }

    public void deleteObjects(String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        prepareDelete().fromTable(tableName).where(condition).orderBy(order).limit(limit).execute();
    }

    public void deleteObjects(String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareDelete().fromTable(tableName).where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void deleteObjects(String tableName, OrderingTerm order, long limit) throws WCDBException {
        prepareDelete().fromTable(tableName).orderBy(order).limit(limit).execute();
    }

    public void deleteObjects(String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareDelete().fromTable(tableName).orderBy(order).limit(limit).offset(offset).execute();
    }

    public <T> void updateObject(T object, Field<T> field, String tableName) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(field).toObject(object).execute();
    }

    public <T> void updateObject(T object, Field<T> field, String tableName, Expression condition) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(field).toObject(object).where(condition).execute();
    }

    public <T> void updateObject(T object, Field<T> field, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(field).toObject(object).where(condition).orderBy(order).limit(limit).execute();
    }

    public <T> void updateObject(T object, Field<T> field, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(field).toObject(object).where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public <T> void updateObject(T object, Field<T> field, String tableName, OrderingTerm order, long limit) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(field).toObject(object).orderBy(order).limit(limit).execute();
    }

    public <T> void updateObject(T object, Field<T> field, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(field).toObject(object).orderBy(order).limit(limit).offset(offset).execute();
    }

    public <T> void updateObject(T object, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(fields).toObject(object).execute();
    }

    public <T> void updateObject(T object, Field<T>[] fields, String tableName, Expression condition) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(fields).toObject(object).where(condition).execute();
    }

    public <T> void updateObject(T object, Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(fields).toObject(object).where(condition).orderBy(order).limit(limit).execute();
    }

    public <T> void updateObject(T object, Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(fields).toObject(object).where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public <T> void updateObject(T object, Field<T>[] fields, String tableName, OrderingTerm order, long limit) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(fields).toObject(object).orderBy(order).limit(limit).execute();
    }

    public <T> void updateObject(T object, Field<T>[] fields, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        this.<T>prepareUpdate().table(tableName).set(fields).toObject(object).orderBy(order).limit(limit).offset(offset).execute();
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).firstObject();
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName, Expression condition) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).firstObject();
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).firstObject();
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long offset) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).limit(1).offset(offset).firstObject();
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName, OrderingTerm order) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).firstObject();
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName, OrderingTerm order, long offset) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).limit(1).offset(offset).firstObject();
    }

    public <T> ArrayList<T> getAllObjects(Field<T>[] fields, String tableName) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).allObjects();
    }

    public <T> ArrayList<T> getAllObjects(Field<T>[] fields, String tableName, Expression condition) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).allObjects();
    }

    public <T> ArrayList<T> getAllObjects(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).allObjects();
    }

    public <T> ArrayList<T> getAllObjects(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).limit(limit).allObjects();
    }

    public <T> ArrayList<T> getAllObjects(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset).allObjects();
    }

    public <T> ArrayList<T> getAllObjects(Field<T>[] fields, String tableName, OrderingTerm order) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).allObjects();
    }

    public <T> ArrayList<T> getAllObjects(Field<T>[] fields, String tableName, OrderingTerm order, long limit) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).limit(limit).allObjects();
    }

    public <T> ArrayList<T> getAllObjects(Field<T>[] fields, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).limit(limit).offset(offset).allObjects();
    }
}

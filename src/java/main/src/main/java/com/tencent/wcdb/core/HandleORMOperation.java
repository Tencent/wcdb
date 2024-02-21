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
package com.tencent.wcdb.core;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.chaincall.Delete;
import com.tencent.wcdb.chaincall.Insert;
import com.tencent.wcdb.chaincall.Select;
import com.tencent.wcdb.chaincall.Update;
import com.tencent.wcdb.orm.Field;
import com.tencent.wcdb.orm.TableBinding;
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.OrderingTerm;
import com.tencent.wcdb.winq.StatementDropIndex;
import com.tencent.wcdb.winq.StatementDropTable;

import java.util.Collection;
import java.util.List;

public abstract class HandleORMOperation extends HandleOperation{
    abstract Database getDatabase();

    /**
     * Create table and indexes from ORM binding if not exists.
     * It will run embedded transaction, and add newly defined columns automatically.
     * The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     * @param tableName This would be the name of the table and the prefix of the index names.
     * @param binding ORM binding of table.
     * @throws WCDBException if any error occurs.
     */
    public <T> void createTable(String tableName, TableBinding<T> binding) throws WCDBException {
        Handle handle = getHandle(true);
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

    /**
     * Create virtual table from ORM if not exists.
     * @see <a href="http://www.sqlite.org/vtab.html">http://www.sqlite.org/vtab.html</a>
     * @param tableName The name of the virtual table to be created.
     * @param binding ORM binding of table.
     * @throws WCDBException if any error occurs.
     */
    public <T> void createVirtualTable(String tableName, TableBinding<T> binding) throws WCDBException {
        Handle handle = getHandle(true);
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

    /**
     * Check the existence of the table.
     * @param tableName The name of the table to be checked.
     * @return true if table exists.
     * @throws WCDBException if any error occurs.
     */
    public boolean tableExist(String tableName) throws WCDBException {
        Handle handle = getHandle(false);
        int ret = Handle.tableExist(handle.cppObj, tableName);
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

    /**
     * Get a wrapper from an existing table.
     * @param tableName The name of the table.
     * @param binding The ORM binding of table.
     * @return Table object.
     */
    public <T> Table<T> getTable(String tableName, TableBinding<T> binding) {
        return new Table<T>(tableName, binding, getDatabase());
    }

    /**
     * Drop table if exists.
     * @param tableName The name of the table to be dropped.
     * @throws WCDBException if any error occurs.
     */
    public void dropTable(String tableName) throws WCDBException {
        execute(new StatementDropTable().dropTable(tableName).ifExist());
    }

    /**
     * Drop index if exists.
     * @param indexName The name of the index to be dropped.
     * @throws WCDBException if any error occurs.
     */
    public void dropIndex(String indexName) throws WCDBException {
        execute(new StatementDropIndex().dropIndex(indexName).ifExist());
    }

    /**
     * Generate a {@link Insert} to do an insertion or replacement.
     * @return An {@link Insert} object.
     */
    public <T> Insert<T> prepareInsert() {
        Insert<T> insert = new Insert<T>(getHandle(true), false, autoInvalidateHandle());
        return insert;
    }

    /**
     * Generate a {@link Update} to do an update.
     * @return An {@link Update} object.
     */
    public <T> Update<T> prepareUpdate() {
        Update<T> update = new Update<T>(getHandle(true), false, autoInvalidateHandle());
        return update;
    }

    /**
     * Generate a {@link Select} to do an object selection.
     * @return An {@link Select} object.
     */
    public <T> Select<T> prepareSelect() {
        Select<T> select = new Select<T>(getHandle(false), false, autoInvalidateHandle());
        return select;
    }

    /**
     * Generate a {@link Delete} to do a deletion.
     * @return An {@link Delete} object.
     */
    public Delete prepareDelete() {
        Delete delete = new Delete(getHandle(true), false, autoInvalidateHandle());
        return delete;
    }

    /**
     * Execute inserting with one object on specific(or all) fields.
     * @param object The object to insert.
     * @param fields specific(or all) fields.
     * @param tableName The table to insert.
     * @throws WCDBException if any error occurs.
     */
    public <T> void insertObject(T object, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().intoTable(tableName).value(object).onFields(fields).execute();
    }

    /**
     * Execute inserting with one object on specific(or all) fields.
     * It will replace the original row while they have same primary key or row id.
     * @param object The object to insert.
     * @param fields specific(or all) fields.
     * @param tableName The table to insert.
     * @throws WCDBException if any error occurs.
     */
    public <T> void insertOrReplaceObject(T object, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().orReplace().intoTable(tableName).value(object).onFields(fields).execute();
    }

    /**
     * Execute inserting with one object on specific(or all) fields.
     * It will ignore the object while there already exists the same primary key or row id in current table.
     * @param object The object to insert.
     * @param fields specific(or all) fields.
     * @param tableName The table to insert.
     * @throws WCDBException if any error occurs.
     */
    public <T> void insertOrIgnoreObject(T object, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().orIgnore().intoTable(tableName).value(object).onFields(fields).execute();
    }

    /**
     * Execute inserting with multi objects on specific(or all) fields.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     * @param objects The objects to insert.
     * @param fields specific(or all) fields.
     * @param tableName The table to insert.
     * @throws WCDBException if any error occurs.
     */
    public <T> void insertObjects(Collection<T> objects, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().intoTable(tableName).values(objects).onFields(fields).execute();
    }

    /**
     * Execute inserting with multi objects on specific(or all) fields.
     * It will replace the original row while they have same primary key or row id.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     * @param objects The objects to insert.
     * @param fields specific(or all) fields.
     * @param tableName The table to insert.
     * @throws WCDBException if any error occurs.
     */
    public <T> void insertOrReplaceObjects(Collection<T> objects, Field<T>[] fields, String tableName) throws WCDBException {
        this.<T>prepareInsert().orReplace().intoTable(tableName).values(objects).onFields(fields).execute();
    }

    /**
     * Execute inserting with multi objects on specific(or all) fields.
     * It will ignore the object while there already exists the same primary key or row id in current table.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     * @param objects The objects to insert.
     * @param fields specific(or all) fields.
     * @param tableName The table to insert.
     * @throws WCDBException if any error occurs.
     */
    public <T> void insertOrIgnoreObjects(Collection<T> objects, Field<T>[] fields, String tableName) throws WCDBException {
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
    public <T, R extends T> R getFirstObject(Field<T>[] fields, String tableName, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).firstObject(cls);
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName, Expression condition) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).firstObject();
    }
    public <T, R extends T> R getFirstObject(Field<T>[] fields, String tableName, Expression condition, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).firstObject(cls);
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).firstObject();
    }
    public <T, R extends T> R getFirstObject(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).firstObject(cls);
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long offset) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).limit(1).offset(offset).firstObject();
    }
    public <T, R extends T> R getFirstObject(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long offset, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).limit(1).offset(offset).firstObject(cls);
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName, OrderingTerm order) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).firstObject();
    }
    public <T, R extends T> R getFirstObject(Field<T>[] fields, String tableName, OrderingTerm order, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).firstObject(cls);
    }

    public <T> T getFirstObject(Field<T>[] fields, String tableName, OrderingTerm order, long offset) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).limit(1).offset(offset).firstObject();
    }
    public <T, R extends T> R getFirstObject(Field<T>[] fields, String tableName, OrderingTerm order, long offset, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).limit(1).offset(offset).firstObject(cls);
    }

    public <T> List<T> getAllObjects(Field<T>[] fields, String tableName) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).allObjects();
    }
    public <T, R extends T> List<R> getAllObjects(Field<T>[] fields, String tableName, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).allObjects(cls);
    }

    public <T> List<T> getAllObjects(Field<T>[] fields, String tableName, Expression condition) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).allObjects();
    }
    public <T, R extends T> List<R> getAllObjects(Field<T>[] fields, String tableName, Expression condition, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).allObjects(cls);
    }

    public <T> List<T> getAllObjects(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).allObjects();
    }
    public <T, R extends T> List<R> getAllObjects(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).allObjects(cls);
    }

    public <T> List<T> getAllObjects(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).limit(limit).allObjects();
    }
    public <T, R extends T> List<R> getAllObjects(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long limit, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).limit(limit).allObjects(cls);
    }

    public <T> List<T> getAllObjects(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset).allObjects();
    }
    public <T, R extends T> List<R> getAllObjects(Field<T>[] fields, String tableName, Expression condition, OrderingTerm order, long limit, long offset, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).where(condition).orderBy(order).limit(limit).offset(offset).allObjects(cls);
    }

    public <T> List<T> getAllObjects(Field<T>[] fields, String tableName, OrderingTerm order) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).allObjects();
    }
    public <T, R extends T> List<R> getAllObjects(Field<T>[] fields, String tableName, OrderingTerm order, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).allObjects(cls);
    }

    public <T> List<T> getAllObjects(Field<T>[] fields, String tableName, OrderingTerm order, long limit) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).limit(limit).allObjects();
    }
    public <T, R extends T> List<R> getAllObjects(Field<T>[] fields, String tableName, OrderingTerm order, long limit, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).limit(limit).allObjects(cls);
    }

    public <T> List<T> getAllObjects(Field<T>[] fields, String tableName, OrderingTerm order, long limit, long offset) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).limit(limit).offset(offset).allObjects();
    }
    public <T, R extends T> List<R> getAllObjects(Field<T>[] fields, String tableName, OrderingTerm order, long limit, long offset, Class<R> cls) throws WCDBException {
        return this.<T>prepareSelect().select(fields).from(tableName).orderBy(order).limit(limit).offset(offset).allObjects(cls);
    }
}

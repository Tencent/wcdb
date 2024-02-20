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

import com.tencent.wcdb.chaincall.Delete;
import com.tencent.wcdb.chaincall.Insert;
import com.tencent.wcdb.chaincall.Select;
import com.tencent.wcdb.chaincall.Update;
import com.tencent.wcdb.orm.Field;
import com.tencent.wcdb.orm.TableBinding;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.OrderingTerm;

import java.util.Collection;
import java.util.List;

public class TableORMOperation<T> extends TableOperation {
    TableBinding<T> binding = null;

    TableORMOperation() {
        super();
    }

    /**
     * Get orm binding.
     */
    public TableBinding<T> getBinding() {
        return binding;
    }

    /**
     * Generate a {@link Insert} to do an insertion or replacement.
     * @return An {@link Insert} object.
     */
    public Insert<T> prepareInsert() {
        Insert<T> insert = new Insert<T>(database.getHandle(true), false, true);
        insert.intoTable(tableName);
        return insert;
    }

    /**
     * Generate a {@link Update} to do an update.
     * @return An {@link Update} object.
     */
    public Update<T> prepareUpdate() {
        Update<T> update = new Update<T>(database.getHandle(true), false, true);
        update.table(tableName);
        return update;
    }

    /**
     * Generate a {@link Select} to do an object selection.
     * @return An {@link Select} object.
     */
    public Select<T> prepareSelect() {
        Select<T> select = new Select<T>(database.getHandle(false), false, true);
        select.from(tableName);
        return select;
    }

    /**
     * Generate a {@link Delete} to do a deletion.
     * @return An {@link Delete} object.
     */
    public Delete prepareDelete() throws WCDBException {
        Delete delete = new Delete(database.getHandle(true), false, true);
        delete.fromTable(tableName);
        return delete;
    }

    /**
     * Execute inserting with one object on all fields.
     * @param object The object to insert..
     * @throws WCDBException if any error occurs.
     */
    public void insertObject(T object) throws WCDBException {
        prepareInsert().value(object).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with one object on specific(or all) fields.
     * @param object The object to insert.
     * @param fields specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertObject(T object, Field<T>[] fields) throws WCDBException {
        prepareInsert().value(object).onFields(fields).execute();
    }

    /**
     * Execute inserting with one object on all fields.
     * It will replace the original row while they have same primary key or row id.
     * @param object The object to insert.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceObject(T object) throws WCDBException {
        prepareInsert().orReplace().value(object).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with one object on specific(or all) fields.
     * It will replace the original row while they have same primary key or row id.
     * @param object The object to insert.
     * @param fields specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceObject(T object, Field<T>[] fields) throws WCDBException {
        prepareInsert().orReplace().value(object).onFields(fields).execute();
    }

    /**
     * Execute inserting with one object on all fields.
     * It will ignore the object while there already exists the same primary key or row id in current table.
     * @param object The object to insert.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreObject(T object) throws WCDBException {
        prepareInsert().orIgnore().value(object).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with one object on specific(or all) fields.
     * It will ignore the object while there already exists the same primary key or row id in current table.
     * @param object The object to insert.
     * @param fields specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreObject(T object, Field<T>[] fields) throws WCDBException {
        prepareInsert().orIgnore().value(object).onFields(fields).execute();
    }

    /**
     * Execute inserting with multi objects on all fields.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     * @param objects The objects to insert.
     * @throws WCDBException if any error occurs.
     */
    public void insertObjects(Collection<T> objects) throws WCDBException {
        prepareInsert().values(objects).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with multi objects on specific(or all) fields.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     * @param objects The objects to insert.
     * @param fields specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertObjects(Collection<T> objects, Field<T>[] fields) throws WCDBException {
        prepareInsert().values(objects).onFields(fields).execute();
    }

    /**
     * Execute inserting with multi objects on all fields.
     * It will replace the original row while they have same primary key or row id.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     * @param objects The objects to insert.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceObjects(Collection<T> objects) throws WCDBException {
        prepareInsert().orReplace().values(objects).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with multi objects on specific(or all) fields.
     * It will replace the original row while they have same primary key or row id.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     * @param objects The objects to insert.
     * @param fields specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceObjects(Collection<T> objects, Field<T>[] fields) throws WCDBException {
        prepareInsert().orReplace().values(objects).onFields(fields).execute();
    }

    /**
     * Execute inserting with multi objects on all fields.
     * It will ignore the object while there already exists the same primary key or row id in current table.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     * @param objects The objects to insert.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreObjects(Collection<T> objects) throws WCDBException {
        prepareInsert().orIgnore().values(objects).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with multi objects on specific(or all) fields.
     * It will ignore the object while there already exists the same primary key or row id in current table.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     * @param objects The objects to insert.
     * @param fields specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreObjects(Collection<T> objects, Field<T>[] fields) throws WCDBException {
        prepareInsert().orIgnore().values(objects).onFields(fields).execute();
    }

    public void deleteObjects() throws WCDBException {
        prepareDelete().execute();
    }

    public void deleteObjects(Expression condition) throws WCDBException {
        prepareDelete().where(condition).execute();
    }

    public void deleteObjects(Expression condition, OrderingTerm order, long limit) throws WCDBException {
        prepareDelete().where(condition).orderBy(order).limit(limit).execute();
    }

    public void deleteObjects(Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareDelete().where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void deleteObjects(OrderingTerm order, long limit) throws WCDBException {
        prepareDelete().orderBy(order).limit(limit).execute();
    }

    public void deleteObjects(OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareDelete().orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(T object) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).execute();
    }

    public void updateObject(T object, Expression condition) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).where(condition).execute();
    }

    public void updateObject(T object, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).where(condition).orderBy(order).limit(limit).execute();
    }

    public void updateObject(T object, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(T object, OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).orderBy(order).limit(limit).execute();
    }

    public void updateObject(T object, OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(T object, Field<T> field) throws WCDBException {
        prepareUpdate().set(field).toObject(object).execute();
    }

    public void updateObject(T object, Field<T> field, Expression condition) throws WCDBException {
        prepareUpdate().set(field).toObject(object).where(condition).execute();
    }

    public void updateObject(T object, Field<T> field, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(field).toObject(object).where(condition).orderBy(order).limit(limit).execute();
    }

    public void updateObject(T object, Field<T> field, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(field).toObject(object).where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(T object, Field<T> field, OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(field).toObject(object).orderBy(order).limit(limit).execute();
    }

    public void updateObject(T object, Field<T> field, OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(field).toObject(object).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(T object, Field<T>[] fields) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).execute();
    }

    public void updateObject(T object, Field<T>[] fields, Expression condition) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).where(condition).execute();
    }

    public void updateObject(T object, Field<T>[] fields, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).where(condition).orderBy(order).limit(limit).execute();
    }

    public void updateObject(T object, Field<T>[] fields, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(T object, Field<T>[] fields, OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).orderBy(order).limit(limit).execute();
    }

    public void updateObject(T object, Field<T>[] fields, OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).orderBy(order).limit(limit).offset(offset).execute();
    }

    public T getFirstObject() throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).firstObject();
    }
    public <R extends T> R getFirstObject(Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).firstObject(cls);
    }

    public T getFirstObject(Expression condition) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).firstObject();
    }
    public <R extends T> R getFirstObject(Expression condition, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).firstObject(cls);
    }

    public T getFirstObject(Expression condition, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).firstObject();
    }
    public <R extends T> R getFirstObject(Expression condition, OrderingTerm order, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).firstObject(cls);
    }

    public T getFirstObject(Expression condition, OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(1).offset(offset).firstObject();
    }
    public <R extends T> R getFirstObject(Expression condition, OrderingTerm order, long offset, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(1).offset(offset).firstObject(cls);
    }

    public T getFirstObject(OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).firstObject();
    }
    public <R extends T> R getFirstObject(OrderingTerm order, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).firstObject(cls);
    }

    public T getFirstObject(OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(1).offset(offset).firstObject();
    }
    public <R extends T> R getFirstObject(OrderingTerm order, long offset, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(1).offset(offset).firstObject(cls);
    }

    public T getFirstObject(Field<T>[] fields) throws WCDBException {
        return prepareSelect().select(fields).firstObject();
    }
    public <R extends T> R getFirstObject(Field<T>[] fields, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).firstObject(cls);
    }

    public T getFirstObject(Field<T>[] fields, Expression condition) throws WCDBException {
        return prepareSelect().select(fields).where(condition).firstObject();
    }
    public <R extends T> R getFirstObject(Field<T>[] fields, Expression condition, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).firstObject(cls);
    }

    public T getFirstObject(Field<T>[] fields, Expression condition, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).firstObject();
    }
    public <R extends T> R getFirstObject(Field<T>[] fields, Expression condition, OrderingTerm order, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).firstObject(cls);
    }

    public T getFirstObject(Field<T>[] fields, Expression condition, OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(1).offset(offset).firstObject();
    }
    public <R extends T> R getFirstObject(Field<T>[] fields, Expression condition, OrderingTerm order, long offset, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(1).offset(offset).firstObject(cls);
    }

    public T getFirstObject(Field<T>[] fields, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).firstObject();
    }
    public <R extends T> R getFirstObject(Field<T>[] fields, OrderingTerm order, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).firstObject(cls);
    }

    public T getFirstObject(Field<T>[] fields, OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(1).offset(offset).firstObject();
    }
    public <R extends T> R getFirstObject(Field<T>[] fields, OrderingTerm order, long offset, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(1).offset(offset).firstObject(cls);
    }

    public List<T> getAllObjects() throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).allObjects(cls);
    }

    public List<T> getAllObjects(Expression condition) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Expression condition, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).allObjects(cls);
    }

    public List<T> getAllObjects(Expression condition, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Expression condition, OrderingTerm order, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).allObjects(cls);
    }

    public List<T> getAllObjects(Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(limit).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Expression condition, OrderingTerm order, long limit, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(limit).allObjects(cls);
    }

    public List<T> getAllObjects(Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(limit).offset(offset).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Expression condition, OrderingTerm order, long limit, long offset, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(limit).offset(offset).allObjects(cls);
    }

    public List<T> getAllObjects(OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).allObjects();
    }
    public <R extends T> List<R> getAllObjects(OrderingTerm order, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).allObjects(cls);
    }

    public List<T> getAllObjects(OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(limit).allObjects();
    }
    public <R extends T> List<R> getAllObjects(OrderingTerm order, long limit, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(limit).allObjects(cls);
    }

    public List<T> getAllObjects(OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(limit).offset(offset).allObjects();
    }
    public <R extends T> List<R> getAllObjects(OrderingTerm order, long limit, long offset, Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(limit).offset(offset).allObjects(cls);
    }

    public List<T> getAllObjects(Field<T>[] fields) throws WCDBException {
        return prepareSelect().select(fields).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Field<T>[] fields, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).allObjects(cls);
    }

    public List<T> getAllObjects(Field<T>[] fields, Expression condition) throws WCDBException {
        return prepareSelect().select(fields).where(condition).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Field<T>[] fields, Expression condition, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).allObjects(cls);
    }

    public List<T> getAllObjects(Field<T>[] fields, Expression condition, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Field<T>[] fields, Expression condition, OrderingTerm order, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).allObjects(cls);
    }

    public List<T> getAllObjects(Field<T>[] fields, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(limit).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Field<T>[] fields, Expression condition, OrderingTerm order, long limit, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(limit).allObjects(cls);
    }

    public List<T> getAllObjects(Field<T>[] fields, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(limit).offset(offset).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Field<T>[] fields, Expression condition, OrderingTerm order, long limit, long offset, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(limit).offset(offset).allObjects(cls);
    }

    public List<T> getAllObjects(Field<T>[] fields, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Field<T>[] fields, OrderingTerm order, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).allObjects(cls);
    }

    public List<T> getAllObjects(Field<T>[] fields, OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(limit).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Field<T>[] fields, OrderingTerm order, long limit, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(limit).allObjects(cls);
    }

    public List<T> getAllObjects(Field<T>[] fields, OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(limit).offset(offset).allObjects();
    }
    public <R extends T> List<R> getAllObjects(Field<T>[] fields, OrderingTerm order, long limit, long offset, Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(limit).offset(offset).allObjects(cls);
    }
}

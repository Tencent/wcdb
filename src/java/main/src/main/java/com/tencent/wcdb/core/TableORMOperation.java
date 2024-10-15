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

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

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
    @NotNull
    public TableBinding<T> getBinding() {
        return binding;
    }

    /**
     * Generate a {@link Insert} to do an insertion or replacement.
     *
     * @return An {@link Insert} object.
     */
    @NotNull
    public Insert<T> prepareInsert() {
        Insert<T> insert = new Insert<T>(database.getHandle(true), false, true);
        insert.intoTable(tableName);
        return insert;
    }

    /**
     * Generate a {@link Update} to do an update.
     *
     * @return An {@link Update} object.
     */
    @NotNull
    public Update<T> prepareUpdate() {
        Update<T> update = new Update<T>(database.getHandle(true), false, true);
        update.table(tableName);
        return update;
    }

    /**
     * Generate a {@link Select} to do an object selection.
     *
     * @return An {@link Select} object.
     */
    @NotNull
    public Select<T> prepareSelect() {
        Select<T> select = new Select<T>(database.getHandle(false), false, true);
        select.from(tableName);
        return select;
    }

    /**
     * Generate a {@link Delete} to do a deletion.
     *
     * @return An {@link Delete} object.
     */
    @NotNull
    public Delete prepareDelete() throws WCDBException {
        Delete delete = new Delete(database.getHandle(true), false, true);
        delete.fromTable(tableName);
        return delete;
    }

    /**
     * Execute inserting with one object on all fields.
     *
     * @param object The object to insert..
     * @throws WCDBException if any error occurs.
     */
    public void insertObject(@Nullable T object) throws WCDBException {
        prepareInsert().value(object).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with one object on specific(or all) fields.
     *
     * @param object The object to insert.
     * @param fields specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertObject(@Nullable T object, @NotNull Field<T>[] fields) throws WCDBException {
        prepareInsert().value(object).onFields(fields).execute();
    }

    /**
     * Execute inserting with one object on all fields.
     * It will replace the original row while they have same primary key or row id.
     *
     * @param object The object to insert.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceObject(@Nullable T object) throws WCDBException {
        prepareInsert().orReplace().value(object).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with one object on specific(or all) fields.
     * It will replace the original row while they have same primary key or row id.
     *
     * @param object The object to insert.
     * @param fields specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceObject(@Nullable T object, @NotNull Field<T>[] fields) throws WCDBException {
        prepareInsert().orReplace().value(object).onFields(fields).execute();
    }

    /**
     * Execute inserting with one object on all fields.
     * It will ignore the object while there already exists the same primary key or row id in current table.
     *
     * @param object The object to insert.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreObject(@Nullable T object) throws WCDBException {
        prepareInsert().orIgnore().value(object).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with one object on specific(or all) fields.
     * It will ignore the object while there already exists the same primary key or row id in current table.
     *
     * @param object The object to insert.
     * @param fields specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreObject(@Nullable T object, @NotNull Field<T>[] fields) throws WCDBException {
        prepareInsert().orIgnore().value(object).onFields(fields).execute();
    }

    /**
     * Execute inserting with multi objects on all fields.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     *
     * @param objects The objects to insert.
     * @throws WCDBException if any error occurs.
     */
    public void insertObjects(@NotNull Collection<T> objects) throws WCDBException {
        prepareInsert().values(objects).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with multi objects on specific(or all) fields.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     *
     * @param objects The objects to insert.
     * @param fields  specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertObjects(@NotNull Collection<T> objects, @NotNull Field<T>[] fields) throws WCDBException {
        prepareInsert().values(objects).onFields(fields).execute();
    }

    /**
     * Execute inserting with multi objects on all fields.
     * It will replace the original row while they have same primary key or row id.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     *
     * @param objects The objects to insert.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceObjects(@NotNull Collection<T> objects) throws WCDBException {
        prepareInsert().orReplace().values(objects).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with multi objects on specific(or all) fields.
     * It will replace the original row while they have same primary key or row id.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     *
     * @param objects The objects to insert.
     * @param fields  specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrReplaceObjects(@NotNull Collection<T> objects, @NotNull Field<T>[] fields) throws WCDBException {
        prepareInsert().orReplace().values(objects).onFields(fields).execute();
    }

    /**
     * Execute inserting with multi objects on all fields.
     * It will ignore the object while there already exists the same primary key or row id in current table.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     *
     * @param objects The objects to insert.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreObjects(@NotNull Collection<T> objects) throws WCDBException {
        prepareInsert().orIgnore().values(objects).onFields(binding.allBindingFields()).execute();
    }

    /**
     * Execute inserting with multi objects on specific(or all) fields.
     * It will ignore the object while there already exists the same primary key or row id in current table.
     * It will run embedded transaction while objects.size()>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     *
     * @param objects The objects to insert.
     * @param fields  specific(or all) fields.
     * @throws WCDBException if any error occurs.
     */
    public void insertOrIgnoreObjects(@NotNull Collection<T> objects, @NotNull Field<T>[] fields) throws WCDBException {
        prepareInsert().orIgnore().values(objects).onFields(fields).execute();
    }

    public void deleteObjects() throws WCDBException {
        prepareDelete().execute();
    }

    public void deleteObjects(@Nullable Expression condition) throws WCDBException {
        prepareDelete().where(condition).execute();
    }

    public void deleteObjects(@Nullable Expression condition, @Nullable OrderingTerm order, long limit) throws WCDBException {
        prepareDelete().where(condition).orderBy(order).limit(limit).execute();
    }

    public void deleteObjects(@Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareDelete().where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void deleteObjects(@Nullable OrderingTerm order, long limit) throws WCDBException {
        prepareDelete().orderBy(order).limit(limit).execute();
    }

    public void deleteObjects(@Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareDelete().orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(@Nullable T object) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).execute();
    }

    public void updateObject(@Nullable T object, @Nullable Expression condition) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).where(condition).execute();
    }

    public void updateObject(@Nullable T object, @Nullable Expression condition, @Nullable OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).where(condition).orderBy(order).limit(limit).execute();
    }

    public void updateObject(@Nullable T object, @Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(@Nullable T object, @Nullable OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).orderBy(order).limit(limit).execute();
    }

    public void updateObject(@Nullable T object, @Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(binding.allBindingFields()).toObject(object).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T> field) throws WCDBException {
        prepareUpdate().set(field).toObject(object).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T> field, @Nullable Expression condition) throws WCDBException {
        prepareUpdate().set(field).toObject(object).where(condition).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T> field, @Nullable Expression condition, @Nullable OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(field).toObject(object).where(condition).orderBy(order).limit(limit).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T> field, @Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(field).toObject(object).where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T> field, @Nullable OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(field).toObject(object).orderBy(order).limit(limit).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T> field, @Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(field).toObject(object).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T>[] fields) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T>[] fields, @Nullable Expression condition) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).where(condition).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).where(condition).orderBy(order).limit(limit).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).where(condition).orderBy(order).limit(limit).offset(offset).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T>[] fields, @Nullable OrderingTerm order, long limit) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).orderBy(order).limit(limit).execute();
    }

    public void updateObject(@Nullable T object, @NotNull Field<T>[] fields, @Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        prepareUpdate().set(fields).toObject(object).orderBy(order).limit(limit).offset(offset).execute();
    }

    @Nullable
    public T getFirstObject() throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@Nullable Expression condition) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@Nullable Expression condition, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@Nullable Expression condition, @Nullable OrderingTerm order, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@Nullable Expression condition, @Nullable OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(1).offset(offset).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@Nullable Expression condition, @Nullable OrderingTerm order, long offset, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(1).offset(offset).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@Nullable OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@Nullable OrderingTerm order, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@Nullable OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(1).offset(offset).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@Nullable OrderingTerm order, long offset, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(1).offset(offset).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@NotNull Field<T>[] fields) throws WCDBException {
        return prepareSelect().select(fields).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@NotNull Field<T>[] fields, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@NotNull Field<T>[] fields, @Nullable Expression condition) throws WCDBException {
        return prepareSelect().select(fields).where(condition).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@NotNull Field<T>[] fields, @Nullable Expression condition, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(1).offset(offset).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, long offset, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(1).offset(offset).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@NotNull Field<T>[] fields, @Nullable OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@NotNull Field<T>[] fields, @Nullable OrderingTerm order, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).firstObject(cls);
    }

    @Nullable
    public T getFirstObject(@NotNull Field<T>[] fields, @Nullable OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(1).offset(offset).firstObject();
    }

    @Nullable
    public <R extends T> R getFirstObject(@NotNull Field<T>[] fields, @Nullable OrderingTerm order, long offset, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(1).offset(offset).firstObject(cls);
    }

    @NotNull
    public List<T> getAllObjects() throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@Nullable Expression condition) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@Nullable Expression condition, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@Nullable Expression condition, @Nullable OrderingTerm order, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@Nullable Expression condition, @Nullable OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(limit).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@Nullable Expression condition, @Nullable OrderingTerm order, long limit, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(limit).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(limit).offset(offset).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(limit).offset(offset).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@Nullable OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@Nullable OrderingTerm order, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@Nullable OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(limit).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@Nullable OrderingTerm order, long limit, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(limit).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(limit).offset(offset).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@Nullable OrderingTerm order, long limit, long offset, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(limit).offset(offset).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@NotNull Field<T>[] fields) throws WCDBException {
        return prepareSelect().select(fields).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition) throws WCDBException {
        return prepareSelect().select(fields).where(condition).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(limit).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, long limit, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(limit).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(limit).offset(offset).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(limit).offset(offset).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@NotNull Field<T>[] fields, @Nullable OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable OrderingTerm order, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@NotNull Field<T>[] fields, @Nullable OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(limit).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable OrderingTerm order, long limit, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(limit).allObjects(cls);
    }

    @NotNull
    public List<T> getAllObjects(@NotNull Field<T>[] fields, @Nullable OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(limit).offset(offset).allObjects();
    }

    @NotNull
    public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable OrderingTerm order, long limit, long offset, @NotNull Class<R> cls) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(limit).offset(offset).allObjects(cls);
    }
}

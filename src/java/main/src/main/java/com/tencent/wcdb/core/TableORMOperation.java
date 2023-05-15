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
import com.tencent.wcdb.winq.ExpressionConvertible;
import com.tencent.wcdb.winq.OrderingTerm;

import java.util.ArrayList;

public class TableORMOperation<T> extends TableOperation {
    TableBinding<T> binding = null;
    TableORMOperation() {
        super();
    }
    public Insert<T> prepareInsert() throws WCDBException {
        Insert<T> insert = new Insert<>(database.getHandle());
        insert.autoInvalidateHandle = true;
        insert.needChanges = false;
        insert.intoTable(tableName);
        return insert;
    }

    public Update<T> prepareUpdate() throws WCDBException {
        Update<T> update = new Update<>(database.getHandle());
        update.autoInvalidateHandle = true;
        update.needChanges = false;
        update.table(tableName);
        return update;
    }

    public Select<T> prepareSelect() throws WCDBException {
        Select<T> select = new Select<>(database.getHandle());
        select.autoInvalidateHandle = true;
        select.needChanges = false;
        select.from(tableName);
        return select;
    }

    public Delete prepareDelete() throws WCDBException {
        Delete delete = new Delete(database.getHandle());
        delete.autoInvalidateHandle = true;
        delete.needChanges = false;
        delete.fromTable(tableName);
        return delete;
    }

    public void insertObject(T object) throws WCDBException {
        prepareInsert().value(object).onFields(binding.allBindingFields()).execute();
    }

    public void insertObject(T object, Field<T>[] fields) throws WCDBException {
        prepareInsert().value(object).onFields(fields).execute();
    }

    public void insertOrReplaceObject(T object) throws WCDBException {
        prepareInsert().orReplace().value(object).onFields(binding.allBindingFields()).execute();
    }

    public void insertOrReplaceObject(T object, Field<T>[] fields) throws WCDBException {
        prepareInsert().orReplace().value(object).onFields(fields).execute();
    }

    public void insertOrIgnoreObject(T object) throws WCDBException {
        prepareInsert().orIgnore().value(object).onFields(binding.allBindingFields()).execute();
    }

    public void insertOrIgnoreObject(T object, Field<T>[] fields) throws WCDBException {
        prepareInsert().orIgnore().value(object).onFields(fields).execute();
    }

    public void insertObjects(T[] objects) throws WCDBException {
        prepareInsert().values(objects).onFields(binding.allBindingFields()).execute();
    }

    public void insertObjects(T[] objects, Field<T>[] fields) throws WCDBException {
        prepareInsert().values(objects).onFields(fields).execute();
    }

    public void insertOrReplaceObjects(T[] objects) throws WCDBException {
        prepareInsert().orReplace().values(objects).onFields(binding.allBindingFields()).execute();
    }

    public void insertOrReplaceObjects(T[] objects, Field<T>[] fields) throws WCDBException {
        prepareInsert().orReplace().values(objects).onFields(fields).execute();
    }

    public void insertOrIgnoreObjects(T[] objects) throws WCDBException {
        prepareInsert().orIgnore().values(objects).onFields(binding.allBindingFields()).execute();
    }

    public void insertOrIgnoreObjects(T[] objects, Field<T>[] fields) throws WCDBException {
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

    public T getFirstObject(Expression condition) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).firstObject();
    }

    public T getFirstObject(Expression condition, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).firstObject();
    }

    public T getFirstObject(Expression condition, OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(1).offset(offset).firstObject();
    }

    public T getFirstObject(OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).firstObject();
    }

    public T getFirstObject(OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(1).offset(offset).firstObject();
    }

    public T getFirstObject(Field<T>[] fields) throws WCDBException {
        return prepareSelect().select(fields).firstObject();
    }

    public T getFirstObject(Field<T>[] fields, Expression condition) throws WCDBException {
        return prepareSelect().select(fields).where(condition).firstObject();
    }

    public T getFirstObject(Field<T>[] fields, Expression condition, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).firstObject();
    }

    public T getFirstObject(Field<T>[] fields, Expression condition, OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(1).offset(offset).firstObject();
    }

    public T getFirstObject(Field<T>[] fields, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).firstObject();
    }

    public T getFirstObject(Field<T>[] fields, OrderingTerm order, long offset) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(1).offset(offset).firstObject();
    }

    public ArrayList<T> getAllObjects() throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).allObjects();
    }

    public ArrayList<T> getAllObjects(Expression condition) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).allObjects();
    }

    public ArrayList<T> getAllObjects(Expression condition, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).allObjects();
    }

    public ArrayList<T> getAllObjects(Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(limit).allObjects();
    }

    public ArrayList<T> getAllObjects(Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).where(condition).orderBy(order).limit(limit).offset(offset).allObjects();
    }

    public ArrayList<T> getAllObjects(OrderingTerm order) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).allObjects();
    }

    public ArrayList<T> getAllObjects(OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(limit).allObjects();
    }

    public ArrayList<T> getAllObjects(OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(binding.allBindingFields()).orderBy(order).limit(limit).offset(offset).allObjects();
    }

    public ArrayList<T> getAllObjects(Field<T>[] fields) throws WCDBException {
        return prepareSelect().select(fields).allObjects();
    }

    public ArrayList<T> getAllObjects(Field<T>[] fields, Expression condition) throws WCDBException {
        return prepareSelect().select(fields).where(condition).allObjects();
    }

    public ArrayList<T> getAllObjects(Field<T>[] fields, Expression condition, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).allObjects();
    }

    public ArrayList<T> getAllObjects(Field<T>[] fields, Expression condition, OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(limit).allObjects();
    }

    public ArrayList<T> getAllObjects(Field<T>[] fields, Expression condition, OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(fields).where(condition).orderBy(order).limit(limit).offset(offset).allObjects();
    }

    public ArrayList<T> getAllObjects(Field<T>[] fields, OrderingTerm order) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).allObjects();
    }

    public ArrayList<T> getAllObjects(Field<T>[] fields, OrderingTerm order, long limit) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(limit).allObjects();
    }

    public ArrayList<T> getAllObjects(Field<T>[] fields, OrderingTerm order, long limit, long offset) throws WCDBException {
        return prepareSelect().select(fields).orderBy(order).limit(limit).offset(offset).allObjects();
    }
}

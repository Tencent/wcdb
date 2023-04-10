//
// Created by qiuwenchen on 2022/9/2.
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

#pragma once
#include "CPPORM.h"
#include "Error.hpp"
#include "HandleOperation.hpp"
#include "MultiObject.hpp"

namespace WCDB {

class HandleORMOperation : public HandleOperation {
public:
#pragma mark - Table
    /**
     @brief Get a wrapper from an existing table.
     @param tableName The name of the table.
     @return WCDB::Table.
     */
    template<class ObjectType>
    Table<ObjectType> getTable(const UnsafeStringView &tableName)
    {
        assertCondition(tableName.length() > 0);
        return Table<ObjectType>(getDatabaseHolder(), tableName);
    }

    /**
     @brief Create virtual table from ORM if not exists.
     @see   http://www.sqlite.org/vtab.html
     @param tableName The name of the virtual table to be created.
     @return True if no error occurs.
     */
    template<class ObjectType>
    bool createVirtualTable(const UnsafeStringView &tableName)
    {
        WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE
        assertCondition(tableName.length() > 0);
        const WCDB::Binding &binding = ObjectType::getObjectRelationBinding();
        return createVirtualTable(tableName, binding);
    }

    /**
     @brief Create table and indexes from ORM if not exists.
     @note  It will run embedded transaction, and add newly defined columns automatically.
            The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @param tableName This would be the name of the table and the prefix of the index names.
     @return True if no error occurs.
     */
    template<class ObjectType>
    bool createTable(const UnsafeStringView &tableName)
    {
        WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE
        assertCondition(tableName.length() > 0);
        const Binding &binding = ObjectType::getObjectRelationBinding();
        return createTable(tableName, binding);
    }

#pragma mark - ChainCall
    /**
     @brief Generate a `WCDB::Insert` to do an insertion or replacement.
     */
    template<class ObjectType>
    Insert<ObjectType> prepareInsert()
    {
        return Insert<ObjectType>(getDatabaseHolder());
    }

    /**
     @brief Generate a `WCDB::Delete` to do a deletion.
     */
    Delete prepareDelete();

    /**
     @brief Generate a `WCDB::Update` to do an update.
     */
    template<class ObjectType>
    Update<ObjectType> prepareUpdate()
    {
        return Update<ObjectType>(getDatabaseHolder());
    }

    /**
     @brief Generate a `WCDB::Select` to do an object selection.
     */
    template<class ObjectType>
    Select<ObjectType> prepareSelect()
    {
        return Select<ObjectType>(getDatabaseHolder());
    }

    /**
     @brief Generate a `WCDB::MultiSelect` to do a cross table selection.
     */
    MultiSelect prepareMultiSelect();

#pragma mark - Insert
    /**
     @brief Execute inserting with multi objects on specific(or all) fields.
     @note  It will run embedded transaction while objs.size>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @return True if no error occurs.
     */
    template<class ObjectType>
    bool insertObjects(const ValueArray<ObjectType> &objs,
                       const UnsafeStringView &table,
                       const Fields &fields = Fields())
    {
        if (objs.size() == 0) {
            return true;
        }
        auto insert = prepareInsert<ObjectType>().intoTable(table).values(objs);
        if (fields.size() > 0) {
            insert.onFields(fields);
        }
        return insert.execute();
    }

    /**
     @brief Execute inserting with multi objects on specific(or all) fields.
     It will replace the original row while they have same primary key or row id.
     @note  It will run embedded transaction while objs.size>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @return True if no error occurs.
     */
    template<class ObjectType>
    bool insertOrReplaceObjects(const ValueArray<ObjectType> &objs,
                                const UnsafeStringView &table,
                                const Fields &fields = Fields())
    {
        if (objs.size() == 0) {
            return true;
        }
        auto insert
        = prepareInsert<ObjectType>().orReplace().intoTable(table).values(objs);
        if (fields.size() > 0) {
            insert.onFields(fields);
        }
        return insert.execute();
    }

    /**
     @brief Execute inserting with multi objects on specific(or all) fields.
     It will ignore the object while there already exists the same primary key or row id in current table.
     @note  It will run embedded transaction while objs.size>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @return True if no error occurs.
     */
    template<class ObjectType>
    bool insertOrIgnoreObjects(const ValueArray<ObjectType> &objs,
                               const UnsafeStringView &table,
                               const Fields &fields = Fields())
    {
        if (objs.size() == 0) {
            return true;
        }
        auto insert
        = prepareInsert<ObjectType>().orIgnore().intoTable(table).values(objs);
        if (fields.size() > 0) {
            insert.onFields(fields);
        }
        return insert.execute();
    }

#pragma mark - Delete
    /**
     @brief Execute deleting.
     @return True if no error occurs.
     */
    bool deleteObjects(const UnsafeStringView &table,
                       const Expression &where = Expression(),
                       const OrderingTerms &orders = OrderingTerms(),
                       const Expression &limit = Expression(),
                       const Expression &offset = Expression());

#pragma mark - Update
    /**
     @brief Execute updating with object on specific(or all) fields.
     @return True if no error occurs.
     */
    template<class ObjectType>
    bool updateObject(const ObjectType &obj,
                      const Fields &fields,
                      const UnsafeStringView &table,
                      const Expression &where = Expression(),
                      const OrderingTerms &orders = OrderingTerms(),
                      const Expression &limit = Expression(),
                      const Expression &offset = Expression())
    {
        auto update = prepareUpdate<ObjectType>().table(table).set(fields).toObject(obj);
        configStatement(update, where, orders, limit, offset);
        return update.execute();
    }

#pragma mark - Select
    /**
     @brief Get an object by specific selecting.
     */
    template<class ObjectType>
    Optional<ObjectType> getFirstObject(const UnsafeStringView &table,
                                        const Expression &where = Expression(),
                                        const OrderingTerms &orders = OrderingTerms(),
                                        const Expression &offset = Expression())
    {
        auto select = prepareSelect<ObjectType>().fromTable(table);
        configStatement(select, where, orders, Expression(1), offset);
        return select.firstObject();
    }

    /**
     @brief Get an object on specific fields by specific selecting.
     */
    template<class ObjectType>
    Optional<ObjectType>
    getFirstObjectWithFields(const UnsafeStringView &table,
                             const ResultFields &resultFields,
                             const Expression &where = Expression(),
                             const OrderingTerms &orders = OrderingTerms(),
                             const Expression &offset = Expression())
    {
        auto select
        = prepareSelect<ObjectType>().fromTable(table).onResultFields(resultFields);
        configStatement(select, where, orders, Expression(1), offset);
        return select.firstObject();
    }

    /**
     @brief Get objects by specific selecting.
     */
    template<class ObjectType>
    OptionalValueArray<ObjectType>
    getAllObjects(const UnsafeStringView &table,
                  const Expression &where = Expression(),
                  const OrderingTerms &orders = OrderingTerms(),
                  const Expression &limit = Expression(),
                  const Expression &offset = Expression())
    {
        auto select = prepareSelect<ObjectType>().fromTable(table);
        configStatement(select, where, orders, limit, offset);
        return select.allObjects();
    }

    /**
     @brief Get objects on specific fields by specific selecting.
     */
    template<class ObjectType>
    OptionalValueArray<ObjectType>
    getAllObjectsWithFields(const UnsafeStringView &table,
                            const ResultFields &resultFields,
                            const Expression &where = Expression(),
                            const OrderingTerms &orders = OrderingTerms(),
                            const Expression &limit = Expression(),
                            const Expression &offset = Expression())
    {
        auto select
        = prepareSelect<ObjectType>().fromTable(table).onResultFields(resultFields);
        configStatement(select, where, orders, limit, offset);
        return select.allObjects();
    }

    /**
     @brief Get first row of objects by specific multi-selecting.
     */
    OptionalMultiObject
    getFirstMultiObject(const ValueArray<StringView> tables,
                        const ResultFields &resultFields,
                        const Expression &where = Expression(),
                        const OrderingTerms &orders = OrderingTerms(),
                        const Expression &limit = Expression(),
                        const Expression &offset = Expression());

    /**
     @brief Get objects by specific multi-selecting.
     */
    OptionalMultiObjectArray
    getAllMultiObjects(const ValueArray<StringView> tables,
                       const ResultFields &resultFields,
                       const Expression &where = Expression(),
                       const OrderingTerms &orders = OrderingTerms(),
                       const Expression &limit = Expression(),
                       const Expression &offset = Expression());

protected:
    bool createTable(const UnsafeStringView &tableName, const Binding &binding);
    bool createVirtualTable(const UnsafeStringView &tableName, const Binding &binding);
    virtual ~HandleORMOperation() override = 0;
};

} //namespace WCDB

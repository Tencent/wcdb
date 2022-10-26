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
#include <WCDB/CPPDeclaration.h>
#include <WCDB/CPPORM.h>
#include <WCDB/Delete.hpp>
#include <WCDB/RecyclableHandle.hpp>
#include <WCDB/TableOperation.hpp>
#include <WCDB/Value.hpp>

namespace WCDB {

template<class ObjectType>
class TableORMOperation : public TableOperation {
public:
#pragma mark - ChainCall
    Insert<ObjectType> prepareInsert()
    {
        return Insert<ObjectType>(getDatabaseHolder()).intoTable(getTableName());
    }

    Delete prepareDelete()
    {
        return Delete(getDatabaseHolder()).fromTable(getTableName());
    }

    Update<ObjectType> prepareUpdate()
    {
        return Update<ObjectType>(getDatabaseHolder()).table(getTableName());
    }

    Select<ObjectType> prepareSelect()
    {
        return Select<ObjectType>(getDatabaseHolder()).fromTable(getTableName());
    }

#pragma mark - Insert
    bool insertObjects(const ValueArray<ObjectType> &objs, const Fields &fields = Fields())
    {
        if (objs.size() == 0) {
            return true;
        }
        auto insert = prepareInsert().values(objs);
        if (fields.size() > 0) {
            insert.onFields(fields);
        }
        return insert.execute();
    }

    bool insertOrReplaceObjects(const ValueArray<ObjectType> &objs,
                                const Fields &fields = Fields())
    {
        if (objs.size() == 0) {
            return true;
        }
        auto insert = prepareInsert().orReplace().values(objs);
        if (fields.size() > 0) {
            insert.onFields(fields);
        }
        return insert.execute();
    }

#pragma mark - Delete
    bool deleteObjects(const Expression &where = Expression(),
                       const OrderingTerms &orders = OrderingTerms(),
                       const Expression &limit = Expression())
    {
        auto delete_ = prepareDelete();
        configStatement(delete_, where, orders, limit, Expression());
        return delete_.execute();
    }

#pragma mark - Update
    bool updateObject(const ObjectType &obj,
                      const Fields &fields,
                      const Expression &where = Expression(),
                      const OrderingTerms &orders = OrderingTerms(),
                      const Expression &limit = Expression(),
                      const Expression &offset = Expression())
    {
        auto update = prepareUpdate().set(fields).toObject(obj);
        configStatement(update, where, orders, limit, offset);
        return update.execute();
    }

#pragma mark - Select
    std::optional<ObjectType>
    getFirstObject(const Expression &where = Expression(),
                   const OrderingTerms &orders = OrderingTerms(),
                   const Expression &offset = Expression())
    {
        auto select = prepareSelect();
        configStatement(select, where, orders, Expression(1), offset);
        return select.firstObject();
    }

    std::optional<ObjectType>
    getFirstObjectWithFields(const ResultFields &resultFields,
                             const Expression &where = Expression(),
                             const OrderingTerms &orders = OrderingTerms(),
                             const Expression &offset = Expression())
    {
        auto select = prepareSelect().onResultFields(resultFields);
        configStatement(select, where, orders, Expression(1), offset);
        return select.firstObject();
    }

    std::optional<ValueArray<ObjectType>>
    getAllObjects(const Expression &where = Expression(),
                  const OrderingTerms &orders = OrderingTerms(),
                  const Expression &limit = Expression(),
                  const Expression &offset = Expression())
    {
        auto select = prepareSelect();
        configStatement(select, where, orders, limit, offset);
        return select.allObjects();
    }

    std::optional<ValueArray<ObjectType>>
    getAllObjectsWithFields(const ResultFields &resultFields,
                            const Expression &where = Expression(),
                            const OrderingTerms &orders = OrderingTerms(),
                            const Expression &limit = Expression(),
                            const Expression &offset = Expression())
    {
        auto select = prepareSelect().onResultFields(resultFields);
        configStatement(select, where, orders, limit, offset);
        return select.allObjects();
    }

protected:
    virtual ~TableORMOperation() override = default;
};

} //namespace WCDB

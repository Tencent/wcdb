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
#include <WCDB/CPPORM.h>
#include <WCDB/Error.hpp>
#include <WCDB/HandleOperation.hpp>

namespace WCDB {

class HandleORMOperation : public HandleOperation {
public:
#pragma mark - Table
    template<class ObjectType>
    Table<ObjectType> getTable(const UnsafeStringView &tableName)
    {
        assertCondition(tableName.length() > 0);
        return Table<ObjectType>(getDatabaseHolder(), tableName);
    }

    template<class ObjectType>
    bool createVirtualTable(const UnsafeStringView &tableName)
    {
        WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE
        assertCondition(tableName.length() > 0);
        return execute(ObjectType::getObjectRelationBinding().generateCreateVirtualTableStatement(
        tableName));
    }

    template<class ObjectType>
    bool createTable(const UnsafeStringView &tableName)
    {
        WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE
        assertCondition(tableName.length() > 0);
        return runTransaction([=](Handle &) {
            auto exists = tableExists(tableName);
            if (!exists.has_value()) {
                return false;
            }
            const Binding &binding = ObjectType::getObjectRelationBinding();
            if (exists.value()) {
                auto optionalColumnNames = getColumns(tableName);
                if (!optionalColumnNames.has_value()) {
                    return false;
                }
                std::set<StringView> &columnNames = optionalColumnNames.value();
                //Check whether the column names exists
                const auto &columnDefs = binding.getColumnDefs();
                for (const auto &columnDef : columnDefs) {
                    auto iter = columnNames.find(columnDef.first);
                    if (iter == columnNames.end()) {
                        //Add new column
                        if (!execute(StatementAlterTable().alterTable(tableName).addColumn(
                            columnDef.second))) {
                            return false;
                        }
                    } else {
                        columnNames.erase(iter);
                    }
                }
                for (const auto &columnName : columnNames) {
                    Error error(Error::Code::Mismatch, Error::Level::Notice, "Skip column");
                    error.infos.insert_or_assign("Table", StringView(tableName));
                    error.infos.insert_or_assign("Column", StringView(columnName));
                    notifyError(error);
                }
            } else {
                if (!execute(binding.generateCreateTableStatement(tableName))) {
                    return false;
                }
            }
            std::list<StatementCreateIndex> createIndexStatements;
            std::list<StatementDropIndex> dropIndexStatements;
            std::tie(createIndexStatements, dropIndexStatements)
            = binding.generateIndexStatements(tableName, !exists.value());
            for (const StatementCreateIndex &statement : createIndexStatements) {
                if (!execute(statement)) {
                    return false;
                }
            }
            for (const StatementDropIndex &statement : dropIndexStatements) {
                if (!execute(statement)) {
                    return false;
                }
            }
            return true;
        });
    }

#pragma mark - ChainCall
    template<class ObjectType>
    Insert<ObjectType> prepareInsert()
    {
        return Insert<ObjectType>(getDatabaseHolder());
    }

    Delete prepareDelete();

    template<class ObjectType>
    Update<ObjectType> prepareUpdate()
    {
        return Update<ObjectType>(getDatabaseHolder());
    }

    template<class ObjectType>
    Select<ObjectType> prepareSelect()
    {
        return Select<ObjectType>(getDatabaseHolder());
    }

#pragma mark - Insert
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

#pragma mark - Delete
    bool deleteObjects(const UnsafeStringView &table,
                       const Expression &where = Expression(),
                       const OrderingTerms &orders = OrderingTerms(),
                       const Expression &limit = Expression(),
                       const Expression &offset = Expression());

#pragma mark - Update
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
    template<class ObjectType>
    std::optional<ObjectType>
    getFirstObject(const UnsafeStringView &table,
                   const Expression &where = Expression(),
                   const OrderingTerms &orders = OrderingTerms(),
                   const Expression &offset = Expression())
    {
        auto select = prepareSelect<ObjectType>().fromTable(table);
        configStatement(select, where, orders, Expression(1), offset);
        return select.firstObject();
    }

    template<class ObjectType>
    std::optional<ObjectType>
    getFirstObjectWithFields(const UnsafeStringView &table,
                             const ResultFields &resultFields,
                             const Expression &where = Expression(),
                             const OrderingTerms &orders = OrderingTerms(),
                             const Expression &offset = Expression())
    {
        auto select
        = prepareSelect<ObjectType>().fromTable(table).onResultColumns(resultFields);
        configStatement(select, where, orders, Expression(1), offset);
        return select.firstObject();
    }

    template<class ObjectType>
    std::optional<ValueArray<ObjectType>>
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

    template<class ObjectType>
    std::optional<ValueArray<ObjectType>>
    getAllObjectsWithFields(const UnsafeStringView &table,
                            const ResultFields &resultFields,
                            const Expression &where = Expression(),
                            const OrderingTerms &orders = OrderingTerms(),
                            const Expression &limit = Expression(),
                            const Expression &offset = Expression())
    {
        auto select
        = prepareSelect<ObjectType>().fromTable(table).onResultColumns(resultFields);
        configStatement(select, where, orders, limit, offset);
        return select.allObjects();
    }

protected:
    virtual ~HandleORMOperation() override = 0;
};

} //namespace WCDB

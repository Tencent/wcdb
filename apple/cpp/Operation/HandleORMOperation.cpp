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

#include <WCDB/Delete.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/HandleORMOperation.hpp>
#include <WCDB/MultiSelect.hpp>

namespace WCDB {

HandleORMOperation::~HandleORMOperation() = default;

Delete HandleORMOperation::prepareDelete()
{
    return Delete(getDatabaseHolder());
}

bool HandleORMOperation::deleteObjects(const UnsafeStringView &table,
                                       const Expression &where,
                                       const OrderingTerms &orders,
                                       const Expression &limit,
                                       const Expression &offset)
{
    auto delete_ = prepareDelete().fromTable(table);
    configStatement(delete_, where, orders, limit, offset);
    return delete_.execute();
}

MultiSelect HandleORMOperation::prepareMultiSelect()
{
    return MultiSelect(getDatabaseHolder());
}

std::optional<MultiObject>
HandleORMOperation::getFirstMultiObject(const ValueArray<StringView> tables,
                                        const ResultFields &resultFields,
                                        const Expression &where,
                                        const OrderingTerms &orders,
                                        const Expression &limit,
                                        const Expression &offset)
{
    auto select = prepareMultiSelect();
    select.onResultFields(resultFields).fromTables(tables);
    configStatement(select, where, orders, limit, offset);
    return select.firstMultiObject();
}

std::optional<ValueArray<MultiObject>>
HandleORMOperation::getAllMultiObjects(const ValueArray<StringView> tables,
                                       const ResultFields &resultFields,
                                       const Expression &where,
                                       const OrderingTerms &orders,
                                       const Expression &limit,
                                       const Expression &offset)
{
    auto select = prepareMultiSelect();
    select.onResultFields(resultFields).fromTables(tables);
    configStatement(select, where, orders, limit, offset);
    return select.allMultiObjects();
}

bool HandleORMOperation::createTable(const UnsafeStringView &tableName, const Binding &binding)
{
    return runTransaction([=](Handle &) {
        auto exists = tableExists(tableName);
        if (!exists.has_value()) {
            return false;
        }
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

} //namespace WCDB

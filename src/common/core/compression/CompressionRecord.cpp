//
// Created by qiuwenchen on 2023/11/26.
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

#include "CompressionRecord.hpp"
#include "CompressionConst.hpp"

namespace WCDB {

const StringView &CompressionRecord::tableName = CompressionRecordTable;
const StringView &CompressionRecord::columnTable = CompressionRecordColumn_Table;
const StringView &CompressionRecord::columnCompressColumns = CompressionRecordColumn_Columns;
const StringView &CompressionRecord::columnRowdid = CompressionRecordColumn_Rowid;

StatementCreateTable CompressionRecord::getCreateTableStatement()
{
    StatementCreateTable createTable;
    createTable.createTable(tableName).ifNotExists();
    createTable.define(
    ColumnDef(columnTable, ColumnType::Text).constraint(ColumnConstraint().primaryKey()));
    createTable.define(ColumnDef(columnCompressColumns, ColumnType::Text)
                       .constraint(ColumnConstraint().notNull()));
    createTable.define(ColumnDef(columnRowdid, ColumnType::Integer));
    createTable.withoutRowID();
    return createTable;
}

StatementInsert CompressionRecord::getInsertValueStatement()
{
    return StatementInsert()
    .insertIntoTable(tableName)
    .orReplace()
    .columns({ columnTable, columnCompressColumns, columnRowdid })
    .values(BindParameter::bindParameters(3));
}

StatementDelete CompressionRecord::getDeleteRecordStatement(const UnsafeStringView &table)
{
    return StatementDelete().deleteFrom(tableName).where(Column(columnTable) == table);
}

StatementUpdate
CompressionRecord::getUpdateRecordStatement(const UnsafeStringView &oldTable,
                                            const UnsafeStringView &newTable)
{
    return StatementUpdate().update(tableName).set(columnTable).to(newTable).where(Column(columnTable) == oldTable);
}

StatementDropTable CompressionRecord::getDropTableStatement()
{
    return StatementDropTable().dropTable(tableName).ifExists();
}

} //namespace WCDB

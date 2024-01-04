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

#pragma once

#include "StringView.hpp"
#include "WINQ.h"

namespace WCDB {

typedef struct CompressionRecord {
    static const StringView& tableName;

    StringView table;
    static const StringView& columnTable;

    StringView columns;
    static const StringView& columnCompressColumns;

    int64_t minCompressedRowid;
    static const StringView& columnRowdid;

    /*
     CREATE TABLE IF NOT EXIST wcdb_builtin_compression_record
     (tableName TEXT PRIMARY KEY, columns TEXT NOT NULL, rowid INTEGER)
     WITHOUT ROWID
     */
    static StatementCreateTable getCreateTableStatement();

    /*
     INSERT OR REPLACE INTO wcdb_builtin_compression_record
     (tableName, columns rowid)
     VALUES(?1, ?2, ?3)
     */
    static StatementInsert getInsertValueStatement();

    /*
     DELETE FROM wcdb_builtin_compression_record
     WHERE tableName == xxx
     */
    static StatementDelete getDeleteRecordStatement(const UnsafeStringView& table);

    /*
     UPDATE wcdb_builtin_compression_record
     SET tableName = newTable
     WHERE tableName == oldTable
     */
    static StatementUpdate getUpdateRecordStatement(const UnsafeStringView& oldTable,
                                                    const UnsafeStringView& newTable);
} CompressionRecord;

} // namespace WCDB

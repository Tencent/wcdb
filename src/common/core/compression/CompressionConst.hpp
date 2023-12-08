//
// Created by 陈秋文 on 2023/11/26.
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

#include "Macro.h"
#include "StringView.hpp"

namespace WCDB {

enum class CompressedType {
    None = 0,
    ZSTDDict,
    ZSTDNormal,
};

#define WCDBMergeCompressionType(compressedType, columnType)                   \
    ((((int) (compressedType)) << 1) | ((columnType) == WCDB::ColumnType::Text ? 0 : 1))
#define WCDBGetCompressedType(mergeType)                                       \
    ((WCDB::CompressedType)((mergeType) >> 1))
#define WCDBGetOriginType(mergeType)                                           \
    ((((mergeType) &0x1) > 0) ? WCDB::ColumnType::BLOB : WCDB::ColumnType::Text)

WCDBLiteralStringDefine(DecompressFunctionName, "wcdb_decompress");

WCDBLiteralStringDefine(CompressionRecordTable, "wcdb_builtin_compression_record");
WCDBLiteralStringDefine(CompressionRecordColumn_Table, "tableName");
WCDBLiteralStringDefine(CompressionRecordColumn_Columns, "columns");
WCDBLiteralStringDefine(CompressionRecordColumn_Rowid, "rowid");

const char CompressionRecordColumnSeperater = ' ';

WCDBLiteralStringDefine(CompressionColumnTypePrefix, "WCDB_CT_")

} //namespace WCDB

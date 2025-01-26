//
// Created by sanhuazhang on 2019/05/02
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

namespace WCDB {

class UnsafeStringView;
class StringView;

namespace Syntax {

enum class ColumnType : signed char {
    Null = 0,
    Integer,
    Float,
    Text,
    BLOB,
};

bool isIntegerColumnType(const UnsafeStringView& type);

enum class CompoundOperator : signed char {
    Union = 1,
    UnionAll,
    Intersect,
    Except,
};

enum class Conflict : signed char {
    Rollback = 1,
    Abort,
    Fail,
    Ignore,
    Replace,
};

enum class JoinOperator : signed char {
    With = 1,
    Join,
    LeftOuterJoin,
    LeftJoin,
    InnerJoin,
    CrossJoin,
    NaturalJoin,
    NaturalLeftOuterJoin,
    NaturalLeftJoin,
    NaturalInnerJoin,
    NaturalCrossJoin,
};

enum class Order : signed char {
    ASC = 0,
    DESC,
};

enum class LimitParameterType : signed char {
    NotSet = 0,
    Offset,
    End,
};

enum class ConflictAction : signed char {
    Replace = 1,
    Rollback,
    Abort,
    Fail,
    Ignore,
};

enum class MatchType : signed char {
    Simple = 1,
    Full,
    Partial,
};

WCDBLiteralStringDefine(masterTable, "sqlite_master");
WCDBLiteralStringDefine(sequenceTable, "sqlite_sequence");
WCDBLiteralStringDefine(mainSchema, "main");
WCDBLiteralStringDefine(tempSchema, "temp");
WCDBLiteralStringDefine(builtinTablePrefix, "sqlite_");
WCDBLiteralStringDefine(builtinWCDBTablePrefix, "wcdb_builtin_");
WCDBLiteralStringDefine(malformedSchemaMsg, "malformed database schema");

} // namespace Syntax

} // namespace WCDB

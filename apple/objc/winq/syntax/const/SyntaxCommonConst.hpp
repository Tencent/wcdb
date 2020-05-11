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

namespace WCDB {

class UnsafeStringView;
class StringView;

namespace Syntax {

enum class ColumnType {
    Null = 0,
    Integer32,
    Integer64,
    Float,
    Text,
    BLOB,
};

bool isIntegerColumnType(const UnsafeStringView& type);

enum class CompoundOperator : int {
    Union = 1,
    UnionAll,
    Intersect,
    Except,
};

enum class Conflict {
    Rollback = 1,
    Abort,
    Fail,
    Ignore,
    Replace,
};

enum class JoinOperator : int {
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

enum class Order : int {
    ASC = 0,
    DESC,
};

enum class LimitParameterType {
    NotSet = 0,
    Offset,
    End,
};

enum class ConflictAction {
    Replace = 1,
    Rollback,
    Abort,
    Fail,
    Ignore,
};
extern const StringView& masterTable;
extern const StringView& mainSchema;
extern const StringView& tempSchema;
extern const StringView& builtinTablePrefix;

} // namespace Syntax

} // namespace WCDB

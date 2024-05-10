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

#include "ExpressionOperable.hpp"
#include "SQL.hpp"

namespace WCDB {

class CompressionTableInfo;

class WCDB_API CoreFunction final {
    friend CompressionTableInfo;

public:
    CoreFunction() = delete;
    CoreFunction(const CoreFunction&) = delete;
    CoreFunction& operator=(const CoreFunction&) = delete;

    static Expression abs(const Expression& expression);
    static Expression changes();
    static Expression char_(const Expressions& expressions);
    static Expression coalesce(const Expressions& expressions);
    static Expression glob(const Expression& pattern, const Expression& string);
    static Expression hex(const Expression& expression);
    static Expression ifNull(const Expression& expression1, const Expression& expression2);
    static Expression instr(const Expression& string, const Expression& occurrence);
    static Expression lastInsertRowID();
    static Expression length(const Expression& expression);
    static Expression like(const Expression& origin, const Expression& pattern);
    static Expression
    like(const Expression& origin, const Expression& pattern, const Expression& escape);
    static Expression likelihood(const Expression& unchanged, const Expression& floating);
    static Expression likely(const Expression& unchanged);
    static Expression lower(const Expression& expression);
    static Expression ltrim(const Expression& expression);
    static Expression ltrim(const Expression& expression, const Expression& removing);
    static Expression max(const Expressions& expressions);
    static Expression min(const Expressions& expressions);
    static Expression nullIf(const Expression& expression1, const Expression& expression2);
    static Expression printf(const Expressions& expressions);
    static Expression quote(const Expression& expression);
    static Expression random();
    static Expression randomBLOB(const Expression& n);
    static Expression
    replace(const Expression& origin, const Expression& target, const Expression& replacement);
    static Expression round(const Expression& expression);
    static Expression round(const Expression& expression, const Expression& digits);
    static Expression rtrim(const Expression& expression);
    static Expression rtrim(const Expression& expression, const Expression& removing);
    static Expression soundex(const Expression& expression);
    static Expression sqliteCompileOptionGet(const Expression& n);
    static Expression sqliteCompileOptionUsed(const Expression& name);
    static Expression sqliteOffset(const Expression& expression);
    static Expression sqliteSourceID();
    static Expression sqliteVersion();
    static Expression substr(const Expression& origin, const Expression& offset);
    static Expression
    substr(const Expression& origin, const Expression& offset, const Expression& length);
    static Expression totalChanges();
    static Expression trim(const Expression& expression);
    static Expression trim(const Expression& expression, const Expression& removing);
    static Expression typeof_(const Expression& expression);
    static Expression unicode(const Expression& expression);
    static Expression unlikely(const Expression& expression);
    static Expression upper(const Expression& expression);
    static Expression zeroBLOB(const Expression& expression);

protected:
    static Expression decompress(const Expression& value, const Expression& compressionType);
};

class WCDB_API CoreFunctionOperable : virtual public ExpressionOperable {
public:
    virtual ~CoreFunctionOperable() override = 0;
    Expression abs() const;
    Expression hex() const;
    Expression length() const;
    Expression lower() const;
    Expression ltrim() const;
    Expression ltrim(const Expression& removing) const;
    Expression replace(const Expression& pattern, const Expression& replacement) const;
    Expression round() const;
    Expression round(const Expression& digits) const;
    Expression rtrim() const;
    Expression rtrim(const Expression& removing) const;
    Expression substr(const Expression& offset) const;
    Expression substr(const Expression& offset, const Expression& length) const;
    Expression trim() const;
    Expression trim(const Expression& removing) const;
    Expression typeof_() const;
    Expression upper() const;
};

} // namespace WCDB

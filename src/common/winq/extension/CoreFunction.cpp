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

#include "CompressionConst.hpp"
#include "WINQ.h"

namespace WCDB {

Expression CoreFunction::abs(const Expression& expression)
{
    static const char* funcName = "abs";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::changes()
{
    static const char* funcName = "changes";
    return Expression::function(StringView::makeConstant(funcName)).invoke();
}

Expression CoreFunction::char_(const Expressions& expressions)
{
    static const char* funcName = "char";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expressions);
}

Expression CoreFunction::coalesce(const Expressions& expressions)
{
    static const char* funcName = "coalesce";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expressions);
}

Expression CoreFunction::glob(const Expression& pattern, const Expression& string)
{
    static const char* funcName = "glob";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ pattern, string });
}

Expression CoreFunction::hex(const Expression& expression)
{
    static const char* funcName = "hex";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::ifNull(const Expression& expression1, const Expression& expression2)
{
    static const char* funcName = "ifnull";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ expression1, expression2 });
}

Expression CoreFunction::instr(const Expression& string, const Expression& occurrence)
{
    static const char* funcName = "instr";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ string, occurrence });
}

Expression CoreFunction::lastInsertRowID()
{
    static const char* funcName = "last_insert_rowid";
    return Expression::function(StringView::makeConstant(funcName)).invoke();
}

Expression CoreFunction::length(const Expression& expression)
{
    static const char* funcName = "length";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::like(const Expression& origin, const Expression& pattern)
{
    static const char* funcName = "like";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ origin, pattern });
}

Expression
CoreFunction::like(const Expression& origin, const Expression& pattern, const Expression& escape)
{
    static const char* funcName = "like";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ origin, pattern, escape });
}

Expression CoreFunction::likelihood(const Expression& unchanged, const Expression& floating)
{
    static const char* funcName = "likelihood";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ unchanged, floating });
}

Expression CoreFunction::likely(const Expression& unchanged)
{
    static const char* funcName = "likely";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(unchanged);
}

Expression CoreFunction::lower(const Expression& expression)
{
    static const char* funcName = "lower";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::ltrim(const Expression& expression)
{
    static const char* funcName = "ltrim";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::ltrim(const Expression& expression, const Expression& removing)
{
    static const char* funcName = "ltrim";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ expression, removing });
}

Expression CoreFunction::max(const Expressions& expressions)
{
    static const char* funcName = "max";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expressions);
}

Expression CoreFunction::min(const Expressions& expressions)
{
    static const char* funcName = "min";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expressions);
}

Expression CoreFunction::nullIf(const Expression& expression1, const Expression& expression2)
{
    static const char* funcName = "nullif";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ expression1, expression2 });
}

Expression CoreFunction::printf(const Expressions& expressions)
{
    static const char* funcName = "printf";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expressions);
}

Expression CoreFunction::quote(const Expression& expression)
{
    static const char* funcName = "quote";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::random()
{
    static const char* funcName = "random";
    return Expression::function(StringView::makeConstant(funcName)).invoke();
}

Expression CoreFunction::randomBLOB(const Expression& n)
{
    static const char* funcName = "randomblob";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(n);
}

Expression CoreFunction::replace(const Expression& origin,
                                 const Expression& target,
                                 const Expression& replacement)
{
    static const char* funcName = "replace";
    return Expression::function(StringView::makeConstant(funcName))
    .invoke()
    .arguments({ origin, target, replacement });
}

Expression CoreFunction::round(const Expression& expression)
{
    static const char* funcName = "round";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::round(const Expression& expression, const Expression& digits)
{
    static const char* funcName = "round";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ expression, digits });
}

Expression CoreFunction::rtrim(const Expression& expression)
{
    static const char* funcName = "rtrim";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::rtrim(const Expression& expression, const Expression& removing)
{
    static const char* funcName = "rtrim";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ expression, removing });
}

Expression CoreFunction::soundex(const Expression& expression)
{
    static const char* funcName = "soundex";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::sqliteCompileOptionGet(const Expression& n)
{
    static const char* funcName = "sqlite_compileoption_get";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(n);
}

Expression CoreFunction::sqliteCompileOptionUsed(const Expression& name)
{
    static const char* funcName = "sqlite_compileoption_used";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(name);
}

Expression CoreFunction::sqliteOffset(const Expression& expression)
{
    static const char* funcName = "sqlite_offset";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::sqliteSourceID()
{
    static const char* funcName = "sqlite_source_id";
    return Expression::function(StringView::makeConstant(funcName)).invoke();
}

Expression CoreFunction::sqliteVersion()
{
    static const char* funcName = "sqlite_version";
    return Expression::function(StringView::makeConstant(funcName)).invoke();
}

Expression CoreFunction::substr(const Expression& origin, const Expression& offset)
{
    static const char* funcName = "substr";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ origin, offset });
}

Expression
CoreFunction::substr(const Expression& origin, const Expression& offset, const Expression& length)
{
    static const char* funcName = "substr";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ origin, offset, length });
}

Expression CoreFunction::totalChanges()
{
    static const char* funcName = "total_changes";
    return Expression::function(StringView::makeConstant(funcName)).invoke();
}

Expression CoreFunction::trim(const Expression& expression)
{
    static const char* funcName = "trim";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::trim(const Expression& expression, const Expression& removing)
{
    static const char* funcName = "trim";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ expression, removing });
}

Expression CoreFunction::typeof_(const Expression& expression)
{
    static const char* funcName = "typeof";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::unicode(const Expression& expression)
{
    static const char* funcName = "unicode";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::unlikely(const Expression& expression)
{
    static const char* funcName = "unlikely";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::upper(const Expression& expression)
{
    static const char* funcName = "upper";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::zeroBLOB(const Expression& expression)
{
    static const char* funcName = "zeroblob";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression CoreFunction::decompress(const Expression& value, const Expression& compressionType)
{
    return Expression::function(DecompressFunctionName).invoke().arguments({ value, compressionType });
}

CoreFunctionOperable::~CoreFunctionOperable() = default;

Expression CoreFunctionOperable::abs() const
{
    return CoreFunction::abs(asExpressionOperand());
}

Expression CoreFunctionOperable::hex() const
{
    return CoreFunction::hex(asExpressionOperand());
}

Expression CoreFunctionOperable::length() const
{
    return CoreFunction::length(asExpressionOperand());
}

Expression CoreFunctionOperable::lower() const
{
    return CoreFunction::lower(asExpressionOperand());
}

Expression CoreFunctionOperable::ltrim() const
{
    return CoreFunction::ltrim(asExpressionOperand());
}

Expression CoreFunctionOperable::ltrim(const Expression& removing) const
{
    return CoreFunction::ltrim(asExpressionOperand(), removing);
}

Expression CoreFunctionOperable::replace(const Expression& pattern,
                                         const Expression& replacement) const
{
    return CoreFunction::replace(asExpressionOperand(), pattern, replacement);
}

Expression CoreFunctionOperable::round() const
{
    return CoreFunction::round(asExpressionOperand());
}

Expression CoreFunctionOperable::round(const Expression& digits) const
{
    return CoreFunction::round(asExpressionOperand(), digits);
}

Expression CoreFunctionOperable::rtrim() const
{
    return CoreFunction::rtrim(asExpressionOperand());
}

Expression CoreFunctionOperable::rtrim(const Expression& removing) const
{
    return CoreFunction::rtrim(asExpressionOperand(), removing);
}

Expression CoreFunctionOperable::substr(const Expression& offset) const
{
    return CoreFunction::substr(asExpressionOperand(), offset);
}

Expression
CoreFunctionOperable::substr(const Expression& offset, const Expression& length) const
{
    return CoreFunction::substr(asExpressionOperand(), offset, length);
}

Expression CoreFunctionOperable::trim() const
{
    return CoreFunction::trim(asExpressionOperand());
}

Expression CoreFunctionOperable::trim(const Expression& removing) const
{
    return CoreFunction::trim(asExpressionOperand(), removing);
}

Expression CoreFunctionOperable::typeof_() const
{
    return CoreFunction::typeof_(asExpressionOperand());
}

Expression CoreFunctionOperable::upper() const
{
    return CoreFunction::upper(asExpressionOperand());
}

} // namespace WCDB

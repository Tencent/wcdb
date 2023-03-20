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

#include "WINQ.h"

namespace WCDB {

StatementPragma::StatementPragma() = default;

StatementPragma::~StatementPragma() = default;

StatementPragma& StatementPragma::pragma(const Pragma& pragma)
{
    syntax().pragma = pragma;
    syntax().switcher = SyntaxType::Switch::NotSet;
    return *this;
}

StatementPragma& StatementPragma::schema(const Schema& schema)
{
    syntax().schema = schema;
    return *this;
}

StatementPragma& StatementPragma::to(const LiteralValue& value)
{
    syntax().switcher = SyntaxType::Switch::To;
    syntax().value = value;
    return *this;
}

StatementPragma& StatementPragma::with(const LiteralValue& value)
{
    syntax().switcher = SyntaxType::Switch::With;
    const StringView& stringValue = value.syntax().stringValue;
    if (stringValue.length() > 2) {
        if ((stringValue.hasPrefix("'") && stringValue.hasSuffix("'"))
            || (stringValue.hasPrefix("[") && stringValue.hasSuffix("]"))
            || (stringValue.hasPrefix("`") && stringValue.hasSuffix("`"))) {
            const UnsafeStringView unQuateValue
            = UnsafeStringView(stringValue.data() + 1, stringValue.length() - 2);
            syntax().value = LiteralValue(unQuateValue);
            return *this;
        }
    }
    syntax().value = value;
    return *this;
}

} // namespace WCDB

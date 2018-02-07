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

#include <WCDB/WINQ.h>

namespace WCDB {

StatementPragma& StatementPragma::withSchema(const std::string& schemaName)
{
    lang::PragmaSTMT& lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementPragma& StatementPragma::pragma(const Pragma& pragmaName)
{
    lang::PragmaSTMT& lang = getMutableLang();
    lang.pragmaName.assign(pragmaName.getDescription());
    lang.value.clear();
    return *this;
}

StatementPragma& StatementPragma::pragma(const Pragma& pragmaName, const LiteralValue& pragmaValue)
{
    lang::PragmaSTMT& lang = getMutableLang();
    lang.pragmaName.assign(pragmaName.getDescription());
    lang.value.assign(pragmaValue.getLang());
    return *this;
}

Statement::Type StatementPragma::getType() const
{
    return Statement::Type::Pragma;
}

} // namespace WCDB

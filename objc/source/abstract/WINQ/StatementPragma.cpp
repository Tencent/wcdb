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

StatementPragma &StatementPragma::withSchema(const std::string &schemaName)
{
    Lang::PragmaSTMT &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementPragma &StatementPragma::pragma(const Pragma &pragmaName)
{
    Lang::PragmaSTMT &lang = getMutableLang();
    lang.pragmaName.assign(pragmaName.getDescription());
    lang.value.clear();
    return *this;
}

StatementPragma &StatementPragma::with(const LiteralValue &pragmaValue)
{
    return to(pragmaValue);
}

StatementPragma &StatementPragma::to(const LiteralValue &pragmaValue)
{
    Lang::PragmaSTMT &lang = getMutableLang();
    lang.value.assign(pragmaValue.getCOWLang());
    return *this;
}

} // namespace WCDB

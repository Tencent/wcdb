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

StatementReindex& StatementReindex::reindex()
{
    LangReindexSTMT& lang = getMutableLang();
    lang.switcher = LangReindexSTMT::Switch::NotSet;
    return *this;
}

StatementReindex& StatementReindex::reindexCollation(const std::string& collationName)
{
    LangReindexSTMT& lang = getMutableLang();
    lang.switcher = LangReindexSTMT::Switch::Collation;
    lang.collationName.assign(collationName);
    return *this;
}

StatementReindex& StatementReindex::reindex(const std::string& tableOrIndexName)
{
    LangReindexSTMT& lang = getMutableLang();
    lang.switcher = LangReindexSTMT::Switch::TableOrIndex;
    lang.tableOrIndexName.assign(tableOrIndexName);
    return *this;
}

StatementReindex& StatementReindex::withSchema(const std::string& schemaName)
{
    LangReindexSTMT& lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

Statement::Type StatementReindex::getType() const
{
    return Statement::Type::Reindex;
}

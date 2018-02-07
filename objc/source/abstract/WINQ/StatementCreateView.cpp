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

StatementCreateView& StatementCreateView::createView(const std::string& viewName, bool ifNotExists, bool temp)
{
    LangCreateViewSTMT& lang = getMutableLang();
    lang.viewName.assign(viewName);
    lang.ifNotExists = ifNotExists;
    lang.temp = temp;
    return *this;
}

StatementCreateView& StatementCreateView::withSchema(const std::string& schemaName)
{
    LangCreateViewSTMT& lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementCreateView& StatementCreateView::on(const std::string& columnName)
{
    LangCreateViewSTMT& lang = getMutableLang();
    lang.columnNames.append(columnName);
    return *this;
}

StatementCreateView& StatementCreateView::on(const std::list<std::string>& columnNames)
{
    LangCreateViewSTMT& lang = getMutableLang();
    for (const std::string& columnName : columnNames) {
        lang.columnNames.append(columnName);
    }
    return *this;
}

StatementCreateView& StatementCreateView::as(const StatementSelect& selectSTMT)
{
    LangCreateViewSTMT& lang = getMutableLang();
    lang.selectSTMT.assign(selectSTMT.getLang());
    return *this;
}

Statement::Type StatementCreateView::getType() const
{
    return Statement::Type::CreateView;
}

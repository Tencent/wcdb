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

#include <WCDB/Assertion.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

StatementCreateView::StatementCreateView()
{
    getMutableLang().ifNotExists = true;
}

StatementCreateView &
StatementCreateView::createView(const std::string &viewName)
{
    Lang::CreateViewSTMT &lang = getMutableLang();
    lang.viewName.assign(viewName);
    return *this;
}

StatementCreateView &
StatementCreateView::withSchema(const std::string &schemaName)
{
    Lang::CreateViewSTMT &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementCreateView &StatementCreateView::ifNotExists(bool ifNotExists)
{
    Lang::CreateViewSTMT &lang = getMutableLang();
    lang.ifNotExists = ifNotExists;
    return *this;
}

StatementCreateView &StatementCreateView::temp(bool temp)
{
    Lang::CreateViewSTMT &lang = getMutableLang();
    lang.temp = temp;
    return *this;
}

StatementCreateView &StatementCreateView::on(const Column &column)
{
    Lang::CreateViewSTMT &lang = getMutableLang();
    lang.columns.append(column.getCOWLang());
    return *this;
}

StatementCreateView &StatementCreateView::on(const std::list<Column> &columns)
{
    Lang::CreateViewSTMT &lang = getMutableLang();
    for (const Column &column : columns) {
        lang.columns.append(column.getCOWLang());
    }
    return *this;
}

StatementCreateView &StatementCreateView::as(const StatementSelect &selectSTMT)
{
    Lang::CreateViewSTMT &lang = getMutableLang();
    lang.selectSTMT.assign(selectSTMT.getCOWLang());
    return *this;
}

} // namespace WCDB

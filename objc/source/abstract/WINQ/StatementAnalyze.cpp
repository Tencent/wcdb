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

StatementAnalyze &StatementAnalyze::analyze()
{
    Lang::AnalyzeSTMT &lang = getMutableLang();
    lang.first.clear();
    lang.second.clear();
    return *this;
}

StatementAnalyze &StatementAnalyze::analyze(const std::string &schemaName,
                                            const std::string &tableOrIndexName)
{
    Lang::AnalyzeSTMT &lang = getMutableLang();
    lang.first.assign(schemaName);
    lang.second.assign(tableOrIndexName);
    return *this;
}

StatementAnalyze &StatementAnalyze::analyze(const std::string &schemaOrTableOrIndexName)
{
    Lang::AnalyzeSTMT &lang = getMutableLang();
    lang.first.assign(schemaOrTableOrIndexName);
    lang.second.clear();
    return *this;
}

} // namespace WCDB

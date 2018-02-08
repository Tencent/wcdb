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

#ifndef TableOrSubquery_hpp
#define TableOrSubquery_hpp

#include <WINQ/Describable.hpp>

namespace WCDB {

class TableOrSubquery : public DescribableWithLang<lang::TableOrSubquery> {
public:
    TableOrSubquery(const std::string &tableName);
    TableOrSubquery(const StatementSelect &selectSTMT);
    TableOrSubquery(const JoinClause &joinClause);
    TableOrSubquery(const std::list<TableOrSubquery> &tableOrSubquerys);

    TableOrSubquery &withSchema(const std::string &schemaName);

    TableOrSubquery &as(const std::string &tableAlias);

    TableOrSubquery &indexedBy(const std::string &indexName);
    TableOrSubquery &notIndexed();

    static TableOrSubquery Function(const std::string &functionName);
    static TableOrSubquery Function(const std::string &functionName,
                                    const Expression &expression);
    static TableOrSubquery Function(const std::string &functionName,
                                    const std::list<Expression> &expressions);

protected:
    TableOrSubquery(
        const lang::copy_on_write_lazy_lang<lang::TableOrSubqueryTableFunction>
            &tableOrSubqueryTableFunction);
};

} // namespace WCDB

#endif /* TableOrSubquery_hpp */

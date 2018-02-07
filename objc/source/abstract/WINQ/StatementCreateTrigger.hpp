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

#ifndef StatementCreateTrigger_hpp
#define StatementCreateTrigger_hpp

#include <WCDB/Describable.hpp>
#include <WCDB/Statement.hpp>

namespace WCDB {

class StatementCreateTrigger
    : public DescribableWithLang<lang::CreateTriggerSTMT>,
      public Statement {
public:
    StatementCreateTrigger &createTrigger(const std::string &triggerName,
                                          bool ifNotExists = true,
                                          bool temp = false);
    StatementCreateTrigger &withSchema(const std::string &schemaName);

    StatementCreateTrigger &before();
    StatementCreateTrigger &after();
    StatementCreateTrigger &insteadOf();

    StatementCreateTrigger &delete_();
    StatementCreateTrigger &insert();
    StatementCreateTrigger &update();
    StatementCreateTrigger &updateOf(const std::string &columnName);
    StatementCreateTrigger &updateOf(const std::list<std::string> &columnNames);

    StatementCreateTrigger &on(const std::string &tableName);
    StatementCreateTrigger &forEachRow();
    StatementCreateTrigger &when(const Expression &expression);

    StatementCreateTrigger &
    byAddingTriggeredStatement(const CRUDStatement &CRUDSTMT);

    virtual Type getType() const override;
};

} // namespace WCDB

#endif /* StatementCreateTrigger_hpp */

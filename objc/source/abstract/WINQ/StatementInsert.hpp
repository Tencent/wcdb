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

#ifndef StatementInsert_hpp
#define StatementInsert_hpp

#include <WCDB/Describable.hpp>
#include <WCDB/Statement.hpp>

namespace WCDB {

class StatementInsert : public DescribableWithLang<lang::InsertSTMT>,
                        public CRUDStatement {
public:
    StatementInsert &with(const WithClause &withClause);
    StatementInsert &insertInto(const std::string &tableName);
    StatementInsert &insertOrReplaceInto(const std::string &tableName);
    StatementInsert &insertOrRollbackInto(const std::string &tableName);
    StatementInsert &insertOrAbortInto(const std::string &tableName);
    StatementInsert &insertOrFailInto(const std::string &tableName);
    StatementInsert &insertOrIgnoreInto(const std::string &tableName);
    StatementInsert &withSchema(const std::string &schemaName);
    StatementInsert &on(const std::string &columnName);
    StatementInsert &on(const std::list<std::string> &columnNames);
    StatementInsert &values(const Expression &value);
    StatementInsert &values(const std::list<Expression> &values);
    StatementInsert &values(const StatementSelect &selectSTMT);
    StatementInsert &defaultValues();

    virtual Type getType() const override;

protected:
    void insertInto(const std::string &tableName,
                    const lang::InsertSTMT::Type &type);
};

} // namespace WCDB

#endif /* StatementInsert_hpp */

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

#ifndef statement_transaction_hpp
#define statement_transaction_hpp

#include <WCDB/statement.hpp>

namespace WCDB {

class StatementTransaction : public Statement {
public:
    enum class Mode : int {
        Defered,
        Immediate,
        Exclusive,
    };
    enum class Type : int {
        Begin,
        Commit,
        Rollback,
    };

    StatementTransaction &begin(StatementTransaction::Mode mode =
                                    StatementTransaction::Mode::Immediate);
    StatementTransaction &commit();
    StatementTransaction &rollback();

    const StatementTransaction::Type &getTransactionType() const;

    virtual Statement::Type getStatementType() const override;

protected:
    StatementTransaction::Type m_type;
};

} //namespace WCDB

#endif /* statement_transaction_hpp */

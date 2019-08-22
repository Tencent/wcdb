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

#include <WCDB/statement_transaction.hpp>

namespace WCDB {

StatementTransaction &
StatementTransaction::begin(StatementTransaction::Mode mode)
{
    m_type = StatementTransaction::Type::Begin;
    m_description.append("BEGIN");
    switch (mode) {
        case StatementTransaction::Mode::Immediate:
            m_description.append(" IMMEDIATE");
            break;
        case StatementTransaction::Mode::Exclusive:
            m_description.append(" EXCLUSIVE");
        default:
            break;
    }
    return *this;
}

StatementTransaction &StatementTransaction::commit()
{
    m_type = StatementTransaction::Type::Commit;
    m_description.append("COMMIT");
    return *this;
}

StatementTransaction &StatementTransaction::rollback()
{
    m_type = StatementTransaction::Type::Rollback;
    m_description.append("ROLLBACK");
    return *this;
}

Statement::Type StatementTransaction::getStatementType() const
{
    return Statement::Type::Transaction;
}

const StatementTransaction::Type &
StatementTransaction::getTransactionType() const
{
    return m_type;
}

} //namespace WCDB

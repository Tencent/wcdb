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

#ifndef Statement_hpp
#define Statement_hpp

#include <WCDB/Describable.hpp>

namespace WCDB {

class Statement : public Describable {
public:
    using Describable::Describable;

    using Type = Lang::STMT::Type;
    virtual Statement::Type getStatementType() const;
};

template<typename T>
class StatementWithLang : public Statement {
public:
    using Statement::Statement;

    static Statement::Type getType() { return T::getType(); }
    Statement::Type getStatementType() const override
    {
        if (!m_cowLang.empty()) {
            return Statement::getStatementType();
        }
        return T::getType();
    };
    T &getMutableLang() { return this->m_cowLang.template get_or_copy<T>(); }
};

class CRUDStatement : public Statement {
public:
    using Statement::Statement;

    virtual Lang::CopyOnWriteLazyLang<Lang::CRUDSTMT> getCRUDSTMT() const = 0;
};

template<typename T>
class CRUDStatementWithLang : public CRUDStatement {
public:
    using CRUDStatement::CRUDStatement;

    static Statement::Type getType() { return T::getType(); }
    Statement::Type getStatementType() const override
    {
        Statement::Type type = Statement::getStatementType();
        if (type == Statement::Type::NotSet) {
            type = T::getType();
        }
        return type;
    };
    T &getMutableLang() { return this->m_cowLang.template get_or_copy<T>(); }

    Lang::CopyOnWriteLazyLang<Lang::CRUDSTMT> getCRUDSTMT() const override
    {
        return m_cowLang;
    }
};

} // namespace WCDB

#endif /* Statement_hpp */

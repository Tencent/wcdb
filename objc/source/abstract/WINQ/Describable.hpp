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

#ifndef Describable_hpp
#define Describable_hpp

#include <WCDB/ColumnType.hpp>
#include <WCDB/Convertible.hpp>
#include <WCDB/Lang.h>

namespace WCDB {

class Describable {
public:
    virtual const std::string &getDescription() const = 0;

protected:
    static const std::string s_empty;
};

template <typename T>
class WithLang {
public:
    WithLang() {}

    WithLang(const Lang::CopyOnWriteLazyLang<T> &lang) : m_lang(lang) {}

    T &getMutableLang() { return m_lang.get_or_copy(); }
    const Lang::CopyOnWriteLazyLang<T> &getLang() const { return m_lang; }

private:
    Lang::CopyOnWriteLazyLang<T> m_lang;
};

template <typename T>
class DescribableWithLang : public Describable, public WithLang<T> {
public:
    DescribableWithLang() {}
    DescribableWithLang(const Lang::CopyOnWriteLazyLang<T> &lang)
        : WithLang<T>(lang)
    {
    }

    virtual const std::string &getDescription() const override
    {
        if (!this->getLang().empty()) {
            return this->getLang().description().get();
        }
        return Describable::s_empty;
    }
};

typedef Lang::Order Order;
typedef Lang::ConflictClause ConflictClause;

class BindParameter;
class Column;
class ColumnConstraint;
class ColumnDef;
class Expression;
class IndexedColumn;
class TableConstraint;
class ModuleArgument;
class WithClause;
class QualifiedTableName;
class OrderingTerm;
class Pragma;
class LiteralValue;
class SelectCore;
class TableOrSubquery;
class JoinClause;
class CommonTableExpression;
class ForeignKeyClause;
class CTETableName;
class JoinConstraint;
class RaiseFunction;
class ResultColumn;

class StatementAlterTable;
class StatementAnalyze;
class StatementAttach;
class StatementBegin;
class StatementCommit;
class StatementCreateIndex;
class StatementCreateTable;
class StatementCreateTrigger;
class StatementCreateView;
class StatementCreateVirtualTable;
class StatementDelete;
class StatementDetach;
class StatementDropIndex;
class StatementDropTable;
class StatementDropTrigger;
class StatementDropView;
class StatementInsert;
class StatementPragma;
class StatementReindex;
class StatementRelease;
class StatementRollback;
class StatementSavepoint;
class StatementSelect;
class StatementUpdate;
class StatementVacuum;

} // namespace WCDB

#endif /* Describable_hpp */

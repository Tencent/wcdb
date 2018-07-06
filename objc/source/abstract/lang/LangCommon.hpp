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

#ifndef LangCommon_hpp
#define LangCommon_hpp

#include <list>

#include <WCDB/CopyOnWriteLazyDescribable.hpp>
#include <WCDB/CopyOnWriteString.hpp>
#include <WCDB/Data.hpp>

#include <WCDB/LangColumnType.hpp>
#include <WCDB/LangConflictClause.hpp>
#include <WCDB/LangOrder.hpp>

#define LangRemedialAssert(cond)                                               \
    WCTRemedialAssert(cond, "WINQ expression is not complete", return description;)

#define LangRemedialFatalError()                                               \
    WCTRemedialFatalError("WINQ expression is not complete", return description;)

namespace WCDB {

namespace Lang {

const std::string &mainSchema();
const std::string &tempSchema();
const std::string &anySchema();

class Lang {
public:
    virtual CopyOnWriteString SQL() const = 0;
};

template<typename T>
class CopyOnWriteLazyLang : public CopyOnWriteLazyDescribable<T, Lang> {
public:
    CopyOnWriteLazyLang() : CopyOnWriteLazyDescribable<T, Lang>() {}

    template<typename OtherType>
    CopyOnWriteLazyLang(const CopyOnWriteLazyLang<OtherType> &o)
    : CopyOnWriteLazyDescribable<T, Lang>(o)
    {
    }

protected:
    CopyOnWriteString calculatedDescription() const override
    {
        return this->get().SQL();
    }
};

template<typename T>
class CopyOnWriteLazyLangList
: public CopyOnWriteLazyDescribable<std::list<CopyOnWriteLazyLang<T>>> {
public:
    bool empty() const
    {
        return CopyOnWriteLazyDescribable<std::list<CopyOnWriteLazyLang<T>>>::empty()
               || this->get().empty();
    }

    void append(const CopyOnWriteLazyLang<T> &element)
    {
        if (!this->empty()) {
            this->get_or_copy().push_back(element);
        } else {
            this->assign({ element });
        }
    }

    template<typename U>
    void append(const CopyOnWriteLazyLang<U> &element)
    {
        CopyOnWriteLazyLang<T> convert;
        convert.assign(element);
        if (!this->empty()) {
            this->get_or_copy().push_back(convert);
        } else {
            this->assign({ convert });
        }
    }

protected:
    CopyOnWriteString calculatedDescription() const override
    {
        std::string description;
        bool comma = false;
        for (const auto &element : this->get()) {
            if (comma) {
                description.append(", ");
            } else {
                comma = true;
            }
            description.append(element.description().get());
        }
        return description;
    }
};

class STMT : public Lang {
public:
    enum class Type : int {
        NotSet = 0,
        AlterTable,
        Analyze,
        Attach,
        Begin,
        Commit,
        CreateIndex,
        CreateTable,
        CreateTrigger,
        CreateView,
        CreateVirtualTable,
        Delete,
        Detach,
        DropIndex,
        DropTable,
        DropTrigger,
        DropView,
        Insert,
        Pragma,
        Reindex,
        Release,
        Rollback,
        Savepoint,
        Select,
        Update,
        Vacuum,
    };
    virtual Type getSTMTType() const = 0;
};

class CRUDSTMT : public STMT {
};

class BindParameter;
class Column;
class ColumnDef;
class ColumnConstraint;
class Expr;
class IndexedColumn;
class TableConstraint;
class ModuleArgument;
class WithClause;
class QualifiedTableName;
class OrderingTerm;
class LiteralValue;
class ResultColumn;
class SelectCore;
class TableOrSubquery;
class JoinClause;
class CommonTableExpression;
class ForeignKeyClause;
class CTETableName;
class JoinOperator;
class JoinConstraint;
class RaiseFunction;

class AnalyzeSTMT;
class AttachSTMT;
class BeginSTMT;
class CommitSTMT;
class CreateIndexSTMT;
class CreateTableSTMT;
class CreateTriggerSTMT;
class CreateViewSTMT;
class CreateVirtualTableSTMT;
class DeleteSTMT;
class DetachSTMT;
class DropIndexSTMT;
class DropTableSTMT;
class DropTriggerSTMT;
class DropViewSTMT;
class InsertSTMT;
class PragmaSTMT;
class ReindexSTMT;
class ReleaseSTMT;
class RollbackSTMT;
class SavepointSTMT;
class SelectSTMT;
class UpdateSTMT;
class VacuumSTMT;

} // namespace Lang

} // namespace WCDB

#endif /* LangCommon_hpp */

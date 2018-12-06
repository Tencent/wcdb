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

#ifndef _WCDB_SYNTAXIDENTIFIER_HPP
#define _WCDB_SYNTAXIDENTIFIER_HPP

#include <WCDB/Shadow.hpp>
#include <WCDB/String.hpp>
#include <functional>
#include <list>
#include <sstream>

#include <WCDB/SyntaxColumnType.hpp>
#include <WCDB/SyntaxCompoundOperator.hpp>
#include <WCDB/SyntaxConflict.hpp>
#include <WCDB/SyntaxJoinOperator.hpp>
#include <WCDB/SyntaxOrder.hpp>

#define SyntaxRemedialAssert(cond)                                             \
    WCTRemedialAssert(cond, "Invalid syntax detected.", return stream.str();)

#define IterateRemedialAssert(cond)                                            \
    WCTRemedialAssert(cond, "Invalid types detected.", return;)

namespace WCDB {

namespace Syntax {

class Identifier : public Cloneable<Identifier> {
public:
    virtual ~Identifier();

    enum class Type {
        Column,
        Schema,
        ColumnDef,
        ColumnConstraint,
        Expression,
        LiteralValue,
        ForeignKeyClause,
        BindParameter,
        RaiseFunction,
        WindowDef,
        Filter,
        IndexedColumn,
        TableConstraint,
        CTETableName,
        WithClause,
        RecursiveCTE,
        CommonTableExpression,
        QualifiedTableName,
        OrderingTerm,
        UpsertClause,
        Pragma,
        JoinClause,
        TableOrSubquery,
        JoinConstraint,
        SelectCore,
        ResultColumn,
        FrameSpec,
        FunctionInvocation,
        WindowFunctionInvocation,

        AlterTableSTMT,
        AnalyzeSTMT,
        AttachSTMT,
        BeginSTMT,
        CommitSTMT,
        RollbackSTMT,
        SavepointSTMT,
        ReleaseSTMT,
        CreateIndexSTMT,
        CreateTableSTMT,
        CreateTriggerSTMT,
        SelectSTMT,
        InsertSTMT,
        DeleteSTMT,
        UpdateSTMT,
        CreateViewSTMT,
        CreateVirtualTableSTMT,
        DetachSTMT,
        DropIndexSTMT,
        DropTableSTMT,
        DropTriggerSTMT,
        DropViewSTMT,
        PragmaSTMT,
        ReindexSTMT,
        VacuumSTMT,
    };
    virtual Type getType() const = 0;

    virtual String getDescription() const = 0;

    Identifier* clone() const override final;

    // Iterable
public:
    typedef std::function<void(Identifier&, void*)> Iterator;
    virtual void iterate(const Iterator& iterator, void* parameter);

protected:
    template<typename T, typename Enable = typename std::enable_if<std::is_base_of<Identifier, T>::value>>
    static void
    listIterate(std::list<T>& identifiers, const Iterator& iterator, void* parameter)
    {
        for (auto& identifier : identifiers) {
            identifier.iterate(iterator, parameter);
        }
    }
    static constexpr const char* space = " ";
};

} // namespace Syntax

} // namespace WCDB

std::ostream& operator<<(std::ostream& stream, const WCDB::Syntax::Identifier& identifiers);

template<typename T, typename Enable = typename std::enable_if<std::is_base_of<WCDB::Syntax::Identifier, T>::value>::type>
std::ostream& operator<<(std::ostream& stream, const std::list<T>& identifiers)
{
    bool comma = false;
    for (const auto& identifier : identifiers) {
        if (comma) {
            stream << ", ";
        } else {
            comma = true;
        }
        stream << identifier;
    }
    return stream;
}

template<typename T, typename Enable = typename std::enable_if<std::is_enum<T>::value>::type>
std::ostream& operator<<(std::ostream& stream, const T& value)
{
    stream << WCDB::Enum::description(value);
    return stream;
}

#endif /* _WCDB_SYNTAXIDENTIFIER_HPP */

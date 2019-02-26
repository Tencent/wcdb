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

#ifndef __WCDB_SYNTAX_IDENTIFIER_HPP
#define __WCDB_SYNTAX_IDENTIFIER_HPP

#include <WCDB/Macro.hpp>
#include <WCDB/Shadow.hpp>
#include <WCDB/String.hpp>
#include <functional>
#include <list>
#include <sstream>
#include <WCDB/SyntaxCommonConst.hpp>

#define __WCDB_SYNTAX_UNION(Type, name)\
static_assert(sizeof(Type) == sizeof(int), "");\
union {\
    Type name;\
    int valid = -1;\
};\
bool isValid() const override final { return valid >= 0; }

#define WCDB_SYNTAX_ENUM(Type, name, ...)\
enum class Type {\
    WCDB_FIRST_ARG(__VA_ARGS__) = 1,\
    WCDB_NON_FIRST_ARGS(__VA_ARGS__)\
};\
__WCDB_SYNTAX_UNION(Type, name)

#define WCDB_DEFAULT_SYNTAX_ENUM(...) WCDB_SYNTAX_ENUM(Switch, switcher, __VA_ARGS__)

namespace WCDB {

namespace Syntax {

class Identifier : public Cloneable<Identifier> {
public:
    virtual ~Identifier() = 0;

    enum class Type {
        Column = 0x00000001,
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

        AlterTableSTMT = 0x00000101,
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

    String getDescription() const;

    Identifier* clone() const override final;
    
    virtual bool isValid() const = 0;
protected:
    virtual String getValidDescription() const = 0;

#pragma mark - Iterable
public:
    typedef std::function<void(Identifier&, bool& stop)> Iterator;
    void iterate(const Iterator& iterator);

protected:
    virtual void iterate(const Iterator& iterator, bool& stop);
    static void
    recursiveIterate(Identifier& identifier, const Iterator& iterator, bool& stop);

    template<typename T, typename Enable = typename std::enable_if<std::is_base_of<Identifier, T>::value>>
    static void
    listIterate(std::list<T>& identifiers, const Iterator& iterator, bool& stop)
    {
        if (!stop) {
            for (auto& identifier : identifiers) {
                recursiveIterate(identifier, iterator, stop);
            }
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

#endif /* __WCDB_SYNTAX_IDENTIFIER_HPP */

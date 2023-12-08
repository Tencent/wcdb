//
// Created by sanhuazhang on 2019/05/02
//

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

#pragma once

#include "Macro.h"
#include "Shadow.hpp"
#include "StringView.hpp"
#include "SyntaxCommonConst.hpp"
#include "WCDBOptional.hpp"
#include <functional>
#include <list>
#include <sstream>

namespace WCDB {

namespace Syntax {

class WCDB_API Identifier : public Cloneable<Identifier> {
public:
    virtual ~Identifier() override = 0;

    enum class Type : signed char {
        Invalid = 0,
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
        ExplainSTMT,
    };
    virtual Type getType() const = 0;

    StringView getDescription() const;

    std::shared_ptr<Identifier> clone() const override final;

    typedef signed char Valid;
    static constexpr const Valid invalid = -1;

    virtual bool isValid() const = 0;

    virtual bool describle(std::ostream& stream) const = 0;

#pragma mark - Iterable
public:
    typedef std::function<void(Identifier&, bool isBegin, bool& stop)> Iterator;
    typedef std::function<void(const Identifier&, bool isBegin, bool& stop)> ConstIterator;

    void iterate(const Iterator& iterator);
    void iterate(const ConstIterator& iterator) const;
    virtual void iterate(const Iterator& iterator, bool& stop);

protected:
    void iterate(const Iterator& iterator, bool isBegin, bool& stop);
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

// The value of Type should always be positive.
#define __WCDB_SYNTAX_UNION_ENUM(Type, nameOfType, nameOfValid)                \
    static_assert(sizeof(Type) == sizeof(Valid), "");                          \
    union {                                                                    \
        Type nameOfType;                                                       \
        Valid nameOfValid = Identifier::invalid;                               \
    };

#define WCDB_SYNTAX_ENUM_UNION(Type, nameOfType)                               \
    __WCDB_SYNTAX_UNION_ENUM(Type, nameOfType, __##nameOfType##Valid);         \
    bool nameOfType##Valid() const                                             \
    {                                                                          \
        return __##nameOfType##Valid >= 0;                                     \
    }

#define WCDB_SYNTAX_UNION_ENUM(Type, nameOfType, First, ...)                   \
    enum class Type : signed char { First = 1, __VA_ARGS__ };                  \
    __WCDB_SYNTAX_UNION_ENUM(Type, nameOfType, __##nameOfType##Valid);         \
    bool nameOfType##Valid() const                                             \
    {                                                                          \
        return __##nameOfType##Valid >= 0;                                     \
    }

#define WCDB_SYNTAX_MAIN_UNION_ENUM(First, ...)                                \
    enum class Switch : signed char { First = 1, __VA_ARGS__ };                \
    __WCDB_SYNTAX_UNION_ENUM(Switch, switcher, __is_valid);                    \
    bool isValid() const override final                                        \
    {                                                                          \
        return __is_valid >= 0;                                                \
    }

#define WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(NameSpace, MemberTag, Type, name)   \
    Type& NameSpace::name()                                                    \
    {                                                                          \
        if (m_##MemberTag != Member::name) {                                   \
            MemberTag##Reset();                                                \
        }                                                                      \
        if (m_##MemberTag == Member::Invalid) {                                \
            new ((void*) std::addressof(m_##name)) Type();                     \
            m_##MemberTag = Member::name;                                      \
        }                                                                      \
        return m_##name;                                                       \
    }                                                                          \
    const Type& NameSpace::name() const                                        \
    {                                                                          \
        if (m_##MemberTag != Member::name) {                                   \
            MemberTag##Reset();                                                \
        }                                                                      \
        if (m_##MemberTag == Member::Invalid) {                                \
            new ((void*) std::addressof(m_##name)) Type();                     \
            m_##MemberTag = Member::name;                                      \
        }                                                                      \
        return m_##name;                                                       \
    }

#define WCDB_SYNTAX_CHECK_OPTIONAL_VALID(value)                                \
    (value.hasValue() && value->isValid())

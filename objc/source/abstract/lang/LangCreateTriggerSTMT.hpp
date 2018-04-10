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

#ifndef LangCreateTriggerSTMT_hpp
#define LangCreateTriggerSTMT_hpp

#include <WCDB/LangCommon.h>

namespace WCDB {

namespace Lang {

class CreateTriggerSTMT : public STMT {
public:
    CreateTriggerSTMT();

    bool temp;
    bool ifNotExists;
    CopyOnWriteString schemaName;
    CopyOnWriteString triggerName;
    enum class Type : int {
        NotSet,
        After,
        Before,
        InsteadOf,
    };
    Type type;

    enum class Operation : int {
        NotSet,
        Delete,
        Insert,
        Update,
    };
    Operation operation;
    CopyOnWriteLazyLangList<Column> columns;

    CopyOnWriteString tableName;

    bool forEachRow;
    CopyOnWriteLazyLang<Expr> expr;

    CopyOnWriteLazyLangList<CRUDSTMT> STMTs;

    virtual CopyOnWriteString SQL() const override;

    virtual STMT::Type getSTMTType() const override;
    static STMT::Type getType();

protected:
    static constexpr const char *TypeName(const Type &type);
    static constexpr const char *OperationName(const Operation &operation);
};

template <>
CopyOnWriteString
CopyOnWriteLazyLangList<CRUDSTMT>::calculatedDescription() const;

} // namespace Lang

} // namespace WCDB

#endif /* LangCreateTriggerSTMT_hpp */

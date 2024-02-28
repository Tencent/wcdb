//
// Created by qiuwenchen on 2022/5/29.
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

#include "ObjectBridge.hpp"
#include "SyntaxForwardDeclaration.h"
#include "SyntaxList.hpp"
#include "WinqBridge.h"

#define WCDBGetCPPSyntaxList(CPPObjType, cppList, rawList, rawCount)           \
    WCDB::SyntaxList<CPPObjType> cppList                                       \
    = WCDB::WinqBridge::createSyntaxList<CPPObjType>(rawList, rawCount);

#define WCDBGetCPPSyntaxListOrReturn(CPPObjType, cppList, rawList, rawCount)   \
    WCDBGetCPPSyntaxList(CPPObjType, cppList, rawList, rawCount);              \
    if (cppList.size() <= 0) {                                                 \
        return;                                                                \
    }

#define WCDBCreateSchemaFromCommonValue(data)                                  \
    WCDB::WinqBridge::createSchema(data)
#define WCDBCreateLiteralValueFromCommonValue(data)                            \
    WCDB::WinqBridge::createLiteralValue(data)
#define WCDBCreateExpressionFromCommonValue(data)                              \
    WCDB::WinqBridge::createExpression(data)
#define WCDBCreateTableOrSubqueryFromCommonValue(data)                         \
    WCDB::WinqBridge::createTableOrSubquery(data)
#define WCDBCreateColumnFromCommonValue(data)                                  \
    WCDB::WinqBridge::createColumn(data)

namespace WCDB {

class WinqBridge {
public:
    static Order changeOrder(enum WCDBSyntaxOrder order);
    static Conflict changeConflict(enum WCDBSyntaxConflictAction action);
    static Syntax::ConflictAction
    changeConflictAction(enum WCDBSyntaxConflictAction action);
    static Syntax::CompoundOperator
    changeCompoundOperator(enum WCDBSyntaxCompoundOperator compound);
    static Syntax::JoinOperator changeJoinOperator(enum WCDBSyntaxJoinOperator join);
    static Syntax::LimitParameterType
    changeLimitParameterType(enum WCDBSyntaxLimitParameterType type);
    static MatchType changeMatchType(enum WCDBSyntaxMatchType type);
    static ColumnType changeColumnType(enum WCDBSyntaxColumnType type);

    template<typename T, typename U>
    static SyntaxList<T> createSyntaxList(const U* itemArr, int itemCount)
    {
        SyntaxList<T> result;
        if (itemArr == nullptr || itemCount <= 0) {
            return result;
        }
        for (int i = 0; i < itemCount; i++) {
            const U& item = itemArr[i];
            T* typedObjName
            = (T*) WCDB::ObjectBridge::extractOriginalCPPObject(item.innerValue);
            if (typedObjName == nullptr) {
                continue;
            }
            result.push_back(*typedObjName);
        }
        return result;
    }

    static Schema createSchema(CPPCommonValue schema);
    static LiteralValue createLiteralValue(CPPCommonValue data);
    static Expression createExpression(CPPCommonValue exp);
    static TableOrSubquery createTableOrSubquery(CPPCommonValue tableOrSubquery);
    static Column createColumn(CPPCommonValue column);
};

} // namespace WCDB

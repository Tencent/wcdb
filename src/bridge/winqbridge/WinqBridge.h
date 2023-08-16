//
// Created by qiuwenchen on 2022/5/26.
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

#include "ObjectBridge.h"

WCDB_EXTERN_C_BEGIN

const char* _Nonnull WCDBWinqGetDescription(CPPObject* _Nonnull statement);

enum WCDBSyntaxOrder {
    WCDBSyntaxOrder_Asc = 1,
    WCDBSyntaxOrder_Desc,
};

enum WCDBSyntaxConflictAction {
    WCDBSyntaxConflictAction_Replace = 1,
    WCDBSyntaxConflictAction_Rollback,
    WCDBSyntaxConflictAction_Abort,
    WCDBSyntaxConflictAction_Fail,
    WCDBSyntaxConflictAction_Ignore,
};

enum WCDBSyntaxCompoundOperator {
    WCDBSyntaxCompoundOperator_Union = 1,
    WCDBSyntaxCompoundOperator_UnionAll,
    WCDBSyntaxCompoundOperator_Intersect,
    WCDBSyntaxCompoundOperator_Except,
};

enum WCDBSyntaxJoinOperator {
    WCDBSyntaxJoinOperator_With = 1,
    WCDBSyntaxJoinOperator_Join,
    WCDBSyntaxJoinOperator_LeftOuterJoin,
    WCDBSyntaxJoinOperator_LeftJoin,
    WCDBSyntaxJoinOperator_InnerJoin,
    WCDBSyntaxJoinOperator_CrossJoin,
    WCDBSyntaxJoinOperator_NaturalJoin,
    WCDBSyntaxJoinOperator_NaturalLeftOuterJoin,
    WCDBSyntaxJoinOperator_NaturalLeftJoin,
    WCDBSyntaxJoinOperator_NaturalInnerJoin,
    WCDBSyntaxJoinOperator_NaturalCrossJoin,
};

enum WCDBSyntaxLimitParameterType {
    WCDBSyntaxLimitParameterType_NotSet = 0,
    WCDBSyntaxLimitParameterType_Offset,
    WCDBSyntaxLimitParameterType_End,
};

enum WCDBSyntaxMatchType {
    WCDBSyntaxMatchType_Simple = 1,
    WCDBSyntaxMatchType_Full,
    WCDBSyntaxMatchType_Partial,
};

enum WCDBSyntaxColumnType {
    WCDBSyntaxColumnType_Null = 0,
    WCDBSyntaxColumnType_Integer,
    WCDBSyntaxColumnType_Float,
    WCDBSyntaxColumnType_Text,
    WCDBSyntaxColumnType_BLOB,
};

enum WCDBSyntaxTransactionType {
    WCDBSyntaxTransactionType_Deferred = 0,
    WCDBSyntaxTransactionType_Immediate,
    WCDBSyntaxTransactionType_Exclusive,
};

bool WCDBStatementNeedToWrite(CPPObject* _Nonnull statement);

WCDBDefineCPPBridgedType(CPPBindParameter);
WCDBDefineCPPBridgedType(CPPColumn);
WCDBDefineCPPBridgedType(CPPColumnConstraint);
WCDBDefineCPPBridgedType(CPPColumnDef);
WCDBDefineCPPBridgedType(CPPCommonTableExpression);
WCDBDefineCPPBridgedType(CPPExpression);
WCDBDefineCPPBridgedType(CPPFilter);
WCDBDefineCPPBridgedType(CPPForeignKey);
WCDBDefineCPPBridgedType(CPPFrameSpec);
WCDBDefineCPPBridgedType(CPPIndexedColumn);
WCDBDefineCPPBridgedType(CPPJoin);
WCDBDefineCPPBridgedType(CPPLiteralValue);
WCDBDefineCPPBridgedType(CPPOrderingTerm);
WCDBDefineCPPBridgedType(CPPPragma);
WCDBDefineCPPBridgedType(CPPQualifiedTable);
WCDBDefineCPPBridgedType(CPPRaiseFunction);
WCDBDefineCPPBridgedType(CPPResultColumn);
WCDBDefineCPPBridgedType(CPPSchema);
WCDBDefineCPPBridgedType(CPPTableConstraint);
WCDBDefineCPPBridgedType(CPPTableOrSubquery);
WCDBDefineCPPBridgedType(CPPUpsert);
WCDBDefineCPPBridgedType(CPPWindowDef);

WCDBDefineCPPBridgedType(CPPStatementAlterTable);
WCDBDefineCPPBridgedType(CPPStatementAnalyze);
WCDBDefineCPPBridgedType(CPPStatementAttach);
WCDBDefineCPPBridgedType(CPPStatementBegin);
WCDBDefineCPPBridgedType(CPPStatementCommit);
WCDBDefineCPPBridgedType(CPPStatementCreateIndex);
WCDBDefineCPPBridgedType(CPPStatementCreateTable);
WCDBDefineCPPBridgedType(CPPStatementCreateTrigger);
WCDBDefineCPPBridgedType(CPPStatementCreateView);
WCDBDefineCPPBridgedType(CPPStatementCreateVirtualTable);
WCDBDefineCPPBridgedType(CPPStatementDelete);
WCDBDefineCPPBridgedType(CPPStatementDetach);
WCDBDefineCPPBridgedType(CPPStatementDropIndex);
WCDBDefineCPPBridgedType(CPPStatementDropTable);
WCDBDefineCPPBridgedType(CPPStatementDropTrigger);
WCDBDefineCPPBridgedType(CPPStatementDropView);
WCDBDefineCPPBridgedType(CPPStatementExplain);
WCDBDefineCPPBridgedType(CPPStatementInsert);
WCDBDefineCPPBridgedType(CPPStatementPragma);
WCDBDefineCPPBridgedType(CPPStatementReIndex);
WCDBDefineCPPBridgedType(CPPStatementRelease);
WCDBDefineCPPBridgedType(CPPStatementRollback);
WCDBDefineCPPBridgedType(CPPStatementSavepoint);
WCDBDefineCPPBridgedType(CPPStatementSelect);
WCDBDefineCPPBridgedType(CPPStatementUpdate);
WCDBDefineCPPBridgedType(CPPStatementVacuum);

WCDB_EXTERN_C_END

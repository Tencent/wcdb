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

#include "ObjectBridge.h"

#include "BindingBridge.h"
#include "ContainerBridge.h"
#include "CoreBridge.h"
#include "DatabaseBridge.h"
#include "ErrorBridge.h"
#include "FTSBridge.h"
#include "HandleBridge.h"
#include "HandleStatementBridge.h"

#include "BindParameterBridge.h"
#include "ColumnBridge.h"
#include "ColumnConstraintBridge.h"
#include "ColumnDefBridge.h"
#include "CommonTableExpressionBridge.h"
#include "ExpressionBridge.h"
#include "ExpressionOperatableBridge.h"
#include "ForeignKeyBridge.h"
#include "FrameSpecBridge.h"
#include "IndexedColumnBridge.h"
#include "JoinBridge.h"
#include "LiteralValueBridge.h"
#include "OrderingTermBridge.h"
#include "PragmaBridge.h"
#include "QualifiedTableBridge.h"
#include "RaiseFunctionBridge.h"
#include "ResultColumnBridge.h"
#include "SchemaBridge.h"
#include "TableConstraintBridge.h"
#include "TableOrSubqueryBridge.h"
#include "UpsertBridge.h"
#include "WindowDefBridge.h"
#include "WinqBridge.h"

#include "StatementAlterTableBridge.h"
#include "StatementAnalyzeBridge.h"
#include "StatementAttachBridge.h"
#include "StatementBeginBridge.h"
#include "StatementCommitBridge.h"
#include "StatementCreateIndexBridge.h"
#include "StatementCreateTableBridge.h"
#include "StatementCreateTriggerBridge.h"
#include "StatementCreateViewBridge.h"
#include "StatementCreateVirtualTableBridge.h"
#include "StatementDeleteBridge.h"
#include "StatementDetachBridge.h"
#include "StatementDropIndexBridge.h"
#include "StatementDropTableBridge.h"
#include "StatementDropTriggerBridge.h"
#include "StatementDropViewBridge.h"
#include "StatementExplainBridge.h"
#include "StatementInsertBridge.h"
#include "StatementPragmaBridge.h"
#include "StatementReindexBridge.h"
#include "StatementReleaseBridge.h"
#include "StatementRollbackBridge.h"
#include "StatementSavepointBridge.h"
#include "StatementSelectBridge.h"
#include "StatementUpdateBridge.h"
#include "StatementVacuumBridge.h"

#include "WCTAPIBridge.h"

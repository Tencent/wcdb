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

#include <Foundation/Foundation.h>

#include <WCDB/ObjectBridge.h>

#include <WCDB/CoreBridge.h>
#include <WCDB/DatabaseBridge.h>
#include <WCDB/ErrorBridge.h>
#include <WCDB/HandleBridge.h>
#include <WCDB/HandleStatementBridge.h>

#include <WCDB/BindParameterBridge.h>
#include <WCDB/ColumnBridge.h>
#include <WCDB/ColumnConstraintBridge.h>
#include <WCDB/ColumnDefBridge.h>
#include <WCDB/CommonTableExpressionBridge.h>
#include <WCDB/ExpressionBridge.h>
#include <WCDB/ExpressionOperatableBridge.h>
#include <WCDB/FilterBridge.h>
#include <WCDB/ForeignKeyBridge.h>
#include <WCDB/FrameSpecBridge.h>
#include <WCDB/IndexedColumnBridge.h>
#include <WCDB/JoinBridge.h>
#include <WCDB/LiteralValueBridge.h>
#include <WCDB/OrderingTermBirdge.h>
#include <WCDB/PragmaBridge.h>
#include <WCDB/QualifiedTableBridge.h>
#include <WCDB/RaiseFunctionBridge.h>
#include <WCDB/ResultColumnBridge.h>
#include <WCDB/SchemaBridge.h>
#include <WCDB/TableConstraintBridge.h>
#include <WCDB/TableOrSubqueryBridge.h>
#include <WCDB/UpsertBridge.h>
#include <WCDB/WindowDefBridge.h>
#include <WCDB/WinqBridge.h>

#include <WCDB/StatementAlterTableBridge.h>
#include <WCDB/StatementAnalyzeBridge.h>
#include <WCDB/StatementAttachBridge.h>
#include <WCDB/StatementBeginBridge.h>
#include <WCDB/StatementCommitBridge.h>
#include <WCDB/StatementCreateIndexBridge.h>
#include <WCDB/StatementCreateTableBridge.h>
#include <WCDB/StatementCreateTriggerBridge.h>
#include <WCDB/StatementCreateViewBridge.h>
#include <WCDB/StatementCreateVirtualTableBridge.h>
#include <WCDB/StatementDeleteBridge.h>
#include <WCDB/StatementDetachBridge.h>
#include <WCDB/StatementDropIndexBridge.h>
#include <WCDB/StatementDropTableBridge.h>
#include <WCDB/StatementDropTriggerBridge.h>
#include <WCDB/StatementDropViewBridge.h>
#include <WCDB/StatementExplainBridge.h>
#include <WCDB/StatementInsertBridge.h>
#include <WCDB/StatementPragmaBridge.h>
#include <WCDB/StatementReindexBridge.h>
#include <WCDB/StatementReleaseBridge.h>
#include <WCDB/StatementRollbackBridge.h>
#include <WCDB/StatementSavepointBridge.h>
#include <WCDB/StatementSelectBridge.h>
#include <WCDB/StatementUpdateBridge.h>
#include <WCDB/StatementVacuumBridge.h>

#define WCDB_SWIFT 1
#include <WCDB/WCTAPIBridge.h>
#include <WCDB/WCTBridgeProperty.h>
#include <WCDB/WCTMacro.h>
#include <WCDB/WCTTableCoding.h>

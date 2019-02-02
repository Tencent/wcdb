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

#ifndef __WCDB_SYNTAX_H
#define __WCDB_SYNTAX_H

// See also: https://sqlite.org/syntaxdiagrams.html

#include <WCDB/SyntaxColumnType.hpp>
#include <WCDB/SyntaxCompoundOperator.hpp>
#include <WCDB/SyntaxConflict.hpp>
#include <WCDB/SyntaxJoinOperator.hpp>
#include <WCDB/SyntaxOrder.hpp>

#include <WCDB/SyntaxCTETableName.hpp>
#include <WCDB/SyntaxColumn.hpp>
#include <WCDB/SyntaxColumnConstraint.hpp>
#include <WCDB/SyntaxColumnDef.hpp>
#include <WCDB/SyntaxCommonTableExpression.hpp>
#include <WCDB/SyntaxExpression.hpp>
#include <WCDB/SyntaxFilter.hpp>
#include <WCDB/SyntaxForeignKeyClause.hpp>
#include <WCDB/SyntaxFrameSpec.hpp>
#include <WCDB/SyntaxFunctionInvocation.hpp>
#include <WCDB/SyntaxIdentifier.hpp>
#include <WCDB/SyntaxIndexedColumn.hpp>
#include <WCDB/SyntaxJoinClause.hpp>
#include <WCDB/SyntaxJoinConstraint.hpp>
#include <WCDB/SyntaxLiteralValue.hpp>
#include <WCDB/SyntaxOrderingTerm.hpp>
#include <WCDB/SyntaxPragma.hpp>
#include <WCDB/SyntaxQualifiedTableName.hpp>
#include <WCDB/SyntaxRecursiveCTE.hpp>
#include <WCDB/SyntaxResultColumn.hpp>
#include <WCDB/SyntaxSchema.hpp>
#include <WCDB/SyntaxSelectCore.hpp>
#include <WCDB/SyntaxTableConstraint.hpp>
#include <WCDB/SyntaxTableOrSubquery.hpp>
#include <WCDB/SyntaxUpsertClause.hpp>
#include <WCDB/SyntaxWindowFunctionInvocation.hpp>
#include <WCDB/SyntaxWithClause.hpp>

#include <WCDB/SyntaxAlterTableSTMT.hpp>
#include <WCDB/SyntaxAnalyzeSTMT.hpp>
#include <WCDB/SyntaxAttachSTMT.hpp>
#include <WCDB/SyntaxBeginSTMT.hpp>
#include <WCDB/SyntaxCommitSTMT.hpp>
#include <WCDB/SyntaxCreateIndexSTMT.hpp>
#include <WCDB/SyntaxCreateTableSTMT.hpp>
#include <WCDB/SyntaxCreateTriggerSTMT.hpp>
#include <WCDB/SyntaxCreateViewSTMT.hpp>
#include <WCDB/SyntaxCreateVirtualTableSTMT.hpp>
#include <WCDB/SyntaxDeleteSTMT.hpp>
#include <WCDB/SyntaxDetachSTMT.hpp>
#include <WCDB/SyntaxDropIndexSTMT.hpp>
#include <WCDB/SyntaxDropTableSTMT.hpp>
#include <WCDB/SyntaxDropTriggerSTMT.hpp>
#include <WCDB/SyntaxDropViewSTMT.hpp>
#include <WCDB/SyntaxInsertSTMT.hpp>
#include <WCDB/SyntaxPragmaSTMT.hpp>
#include <WCDB/SyntaxReindexSTMT.hpp>
#include <WCDB/SyntaxReleaseSTMT.hpp>
#include <WCDB/SyntaxRollbackSTMT.hpp>
#include <WCDB/SyntaxSavepointSTMT.hpp>
#include <WCDB/SyntaxUpdateSTMT.hpp>
#include <WCDB/SyntaxVacuumSTMT.hpp>

#endif /* __WCDB_SYNTAX_H */

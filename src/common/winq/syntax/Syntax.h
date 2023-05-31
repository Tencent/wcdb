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

// See also: https://sqlite.org/syntaxdiagrams.html

// TODO: use double-quotes for all identifiers to accept SQLite keyword. https://sqlite.org/c3ref/keyword_check.html

#include "SyntaxCommonConst.hpp"

#include "SyntaxColumn.hpp"
#include "SyntaxColumnConstraint.hpp"
#include "SyntaxColumnDef.hpp"
#include "SyntaxCommonTableExpression.hpp"
#include "SyntaxExpression.hpp"
#include "SyntaxFilter.hpp"
#include "SyntaxForeignKeyClause.hpp"
#include "SyntaxFrameSpec.hpp"
#include "SyntaxIdentifier.hpp"
#include "SyntaxIndexedColumn.hpp"
#include "SyntaxJoinClause.hpp"
#include "SyntaxJoinConstraint.hpp"
#include "SyntaxLiteralValue.hpp"
#include "SyntaxOrderingTerm.hpp"
#include "SyntaxPragma.hpp"
#include "SyntaxQualifiedTableName.hpp"
#include "SyntaxResultColumn.hpp"
#include "SyntaxSchema.hpp"
#include "SyntaxSelectCore.hpp"
#include "SyntaxSelectSTMT.hpp"
#include "SyntaxTableConstraint.hpp"
#include "SyntaxTableOrSubquery.hpp"
#include "SyntaxUpsertClause.hpp"

#include "SyntaxAlterTableSTMT.hpp"
#include "SyntaxAnalyzeSTMT.hpp"
#include "SyntaxAttachSTMT.hpp"
#include "SyntaxBeginSTMT.hpp"
#include "SyntaxCommitSTMT.hpp"
#include "SyntaxCreateIndexSTMT.hpp"
#include "SyntaxCreateTableSTMT.hpp"
#include "SyntaxCreateTriggerSTMT.hpp"
#include "SyntaxCreateViewSTMT.hpp"
#include "SyntaxCreateVirtualTableSTMT.hpp"
#include "SyntaxDeleteSTMT.hpp"
#include "SyntaxDetachSTMT.hpp"
#include "SyntaxDropIndexSTMT.hpp"
#include "SyntaxDropTableSTMT.hpp"
#include "SyntaxDropTriggerSTMT.hpp"
#include "SyntaxDropViewSTMT.hpp"
#include "SyntaxInsertSTMT.hpp"
#include "SyntaxPragmaSTMT.hpp"
#include "SyntaxReindexSTMT.hpp"
#include "SyntaxReleaseSTMT.hpp"
#include "SyntaxRollbackSTMT.hpp"
#include "SyntaxSavepointSTMT.hpp"
#include "SyntaxUpdateSTMT.hpp"
#include "SyntaxVacuumSTMT.hpp"

#include "SyntaxExplainSTMT.hpp"

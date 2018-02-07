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

#ifndef lang_common_h
#define lang_common_h

#include <assert.h>

#include <WCDB/copy_on_write_data.hpp>
#include <WCDB/copy_on_write_lazy_string_list.hpp>
#include <WCDB/copy_on_write_string.hpp>

#include <WCDB/copy_on_write_lazy_lang.hpp>

#include <WCDB/LangColumnType.hpp>
#include <WCDB/LangConflictClause.hpp>
#include <WCDB/LangOrder.hpp>

class Lang {
public:
    virtual copy_on_write_string SQL() const = 0;
};

class CRUDLang : public Lang {
};

class LangBindParameter;
class LangColumnDef;
class LangColumnConstraint;
class LangExpr;
class LangIndexedColumn;
class LangTableConstraint;
class LangModuleArgument;
class LangWithClause;
class LangQualifiedTableName;
class LangOrderingTerm;
class LangLiteralValue;
class LangResultColumn;
class LangSelectCore;
class LangTableOrSubquery;
class LangJoinClause;
class LangCommonTableExpression;
class LangForeignKeyClause;
class LangCTETableName;
class LangJoinOperator;
class LangJoinConstraint;
class LangRaiseFunction;

class LangAnalyzeSTMT;
class LangAttachSTMT;
class LangBeginSTMT;
class LangCommitSTMT;
class LangCreateIndexSTMT;
class LangCreateTableSTMT;
class LangCreateTriggerSTMT;
class LangCreateViewSTMT;
class LangCreateVirtualTableSTMT;
class LangDeleteSTMT;
class LangDetachSTMT;
class LangDropIndexSTMT;
class LangDropTableSTMT;
class LangDropTriggerSTMT;
class LangDropViewSTMT;
class LangInsertSTMT;
class LangPragmaSTMT;
class LangReindexSTMT;
class LangReleaseSTMT;
class LangRollbackSTMT;
class LangSavepointSTMT;
class LangSelectSTMT;
class LangUpdateSTMT;
class LangVacuumSTMT;

#endif /* lang_common_h */

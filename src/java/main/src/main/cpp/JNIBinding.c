// Created by qiuwenchen on 2023/3/30.
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

#include "BindParameterJNI.h"
#include "BindingJNI.h"
#include "ColumnConstraintJNI.h"
#include "ColumnDefJNI.h"
#include "ColumnJNI.h"
#include "CoreJNI.h"
#include "DatabaseJNI.h"
#include "ErrorJNI.h"
#include "ExpressionJNI.h"
#include "ExpressionOperableJNI.h"
#include "HandleJNI.h"
#include "HandleStatementJNI.h"
#include "IndexedColumnJNI.h"
#include "LiteralValueJNI.h"
#include "OrderingTermJNI.h"
#include "PragmaJNI.h"
#include "QualifiedTableJNI.h"
#include "ResultColumnJNI.h"
#include "SchemaJNI.h"
#include "StatementBeginJNI.h"
#include "StatementCommitJNI.h"
#include "StatementCreateTableJNI.h"
#include "StatementCreateindexJNI.h"
#include "StatementDeleteJNI.h"
#include "StatementDropTableJNI.h"
#include "StatementInsertJNI.h"
#include "StatementPragmaJNI.h"
#include "StatementSelectJNI.h"
#include "StatementUpdateJNI.h"
#include "TableConstraintJNI.h"
#include "TableOrSubqueryJNI.h"
#include "WinqJNI.h"

#define WCDBJNIMultiTypeSignature "IJDLjava/lang/String;"
#define WCDBJNIObjectOrStringSignature "IJLjava/lang/String;"
#define WCDBJNICommonArraySignature "I[J[D[Ljava/lang/String;"
#define WCDBJNIObjectOrStringArraySignature "I[J[Ljava/lang/String;"
#define WCDBJNIMultiTypeArraySignature "[I[J[D[Ljava/lang/String;"

#define WCDBJNIRegister(className, methodList)                                 \
    {                                                                          \
        className, (JNINativeMethod *) &methodList,                            \
        sizeof(methodList) / sizeof(JNINativeMethod)                           \
    }

typedef struct JNIBinding {
    const char *className;
    JNINativeMethod *methods;
    int methodCount;
} JNIBinding;

static const JNINativeMethod g_objectBridgeMethods[] = {
    { "releaseCPPObject", "(J)V", (void *) WCDBJNI(Base, releaseObject) },
};

static const JNINativeMethod g_winqBridgeMethods[] = {
    { "getDescription", "(J)" WCDBJNIStringSignature "", (void *) WCDBJNI(Winq, getDescription) },
};

static const JNINativeMethod g_literalValueMethods[] = {
    { "createCppObj", "(" WCDBJNIMultiTypeSignature ")J", (void *) WCDBJNILiteralValueFuncName(create) },
    { "createCurrentTime", "()J", (void *) WCDBJNILiteralValueFuncName(createWithCurrentTime) },
    { "createCurrentDate", "()J", (void *) WCDBJNILiteralValueFuncName(createWithCurrentDate) },
    { "createCurrentTimeStamp", "()J", (void *) WCDBJNILiteralValueFuncName(createWithCurrentTimestamp) },
};

static const JNINativeMethod g_schemaMethods[] = {
    { "createCppObj", "(" WCDBJNIStringSignature ")J", (void *) WCDBJNISchemaFuncName(createWithName) },
    { "createMainCppObj", "()J", (void *) WCDBJNISchemaFuncName(main) },
    { "createTempCppObj", "()J", (void *) WCDBJNISchemaFuncName(temp) },
};

static const JNINativeMethod g_columnMethods[] = {
    { "createCppObj", "(" WCDBJNIStringSignature "J)J", (void *) WCDBJNIColumnFuncName(createWithName) },
    { "copy", "(J)J", (void *) WCDBJNIColumnFuncName(copy) },
    { "inTable", "(J" WCDBJNIStringSignature ")V", (void *) WCDBJNIColumnFuncName(inTable) },
    { "inSchema", "(JJ)V", (void *) WCDBJNIColumnFuncName(inSchema) },
    { "allColumn", "()J", (void *) WCDBJNIColumnFuncName(createAll) },
    { "rowidColumn", "()J", (void *) WCDBJNIColumnFuncName(createRowId) },
};

static const JNINativeMethod g_bindParameterMethods[] = {
    { "createCppObj", "(I)J", (void *) WCDBJNIBindParameterFuncName(createQuestionSignType) },
    { "createCppObj",
      "(" WCDBJNIStringSignature ")J",
      (void *) WCDBJNIBindParameterFuncName(createColonSignType) },
    { "createCppObjs", "(I)[J", (void *) WCDBJNIBindParameterFuncName(bindParameters) },
    { "atBindParameter",
      "(" WCDBJNIStringSignature ")J",
      (void *) WCDBJNIBindParameterFuncName(createAtSignType) },
    { "dollarBindParameter",
      "(" WCDBJNIStringSignature ")J",
      (void *) WCDBJNIBindParameterFuncName(createDollarSignType) },
};

static const JNINativeMethod g_expressionMethods[] = {
    { "createCppObj", "(IJ)J", (void *) WCDBJNIExpressionFuncName(create) },
    { "createWithFunction",
      "(" WCDBJNIStringSignature ")J",
      (void *) WCDBJNIExpressionFuncName(createWithFunction) },
    { "schema", "(JJ)V", (void *) WCDBJNIExpressionFuncName(setWithSchema) },
    { "distinct", "(J)V", (void *) WCDBJNIExpressionFuncName(distinct) },
    { "invoke", "(J)V", (void *) WCDBJNIExpressionFuncName(invoke) },
    { "invokeAll", "(J)V", (void *) WCDBJNIExpressionFuncName(invokeAll) },
    { "argument", "(J" WCDBJNIMultiTypeSignature ")V", (void *) WCDBJNIExpressionFuncName(setArgument) },
    { "escape", "(J" WCDBJNIStringSignature ")V", (void *) WCDBJNIExpressionFuncName(escapeWith) },
};

static const JNINativeMethod g_expressionOperationMethods[] = {
    { "nullOperate", "(IJZ)J", (void *) WCDBJNIExpressionOperableFuncName(nullOperate) },
    { "binaryOperate",
      "(IJ" WCDBJNIMultiTypeSignature "IZ)J",
      (void *) WCDBJNIExpressionOperableFuncName(binaryOperate) },
    { "betweenOperate",
      "(IJ" WCDBJNIMultiTypeSignature WCDBJNIMultiTypeSignature "Z)J",
      (void *) WCDBJNIExpressionOperableFuncName(betweenOperate) },
    { "in",
      "(IJ" WCDBJNICommonArraySignature "Z)J",
      (void *) WCDBJNIExpressionOperableFuncName(inOperate) },
    { "inTable",
      "(" WCDBJNIObjectOrStringSignature "Z)J",
      (void *) WCDBJNIExpressionOperableFuncName(inTableOperate) },
    { "inFunction",
      "(" WCDBJNIObjectOrStringSignature "Z)J",
      (void *) WCDBJNIExpressionOperableFuncName(inFunctionOperate) },
    { "inSelect", "(IJJZ)J", (void *) WCDBJNIExpressionOperableFuncName(inSelectionOperate) },
    { "collate",
      "(" WCDBJNIObjectOrStringSignature ")J",
      (void *) WCDBJNIExpressionOperableFuncName(collateOperate) },
};

static const JNINativeMethod g_columnConstraintMethods[] = {
    { "createCppObject",
      "(" WCDBJNIStringSignature ")J",
      (void *) WCDBJNIColumnConstraintFuncName(create) },
    { "configPrimaryKey", "(J)V", (void *) WCDBJNIColumnConstraintFuncName(configPrimaryKey) },
    { "configOrder", "(JI)V", (void *) WCDBJNIColumnConstraintFuncName(configOrder) },
    { "configConflictAction", "(JI)V", (void *) WCDBJNIColumnConstraintFuncName(configConflictAction) },
    { "configAutoIncrement", "(J)V", (void *) WCDBJNIColumnConstraintFuncName(configAutoIncrement) },
    { "configNotNull", "(J)V", (void *) WCDBJNIColumnConstraintFuncName(configNotNull) },
    { "configUnique", "(J)V", (void *) WCDBJNIColumnConstraintFuncName(configUnique) },
    { "configCheck", "(JJ)V", (void *) WCDBJNIColumnConstraintFuncName(configCheck) },
    { "defaultTo",
      "(J" WCDBJNIMultiTypeSignature ")V",
      (void *) WCDBJNIColumnConstraintFuncName(configDefaultValue) },
    { "configCollate",
      "(J" WCDBJNIStringSignature ")V",
      (void *) WCDBJNIColumnConstraintFuncName(configCollation) },
    { "configForeignKey", "(JJ)V", (void *) WCDBJNIColumnConstraintFuncName(configForeignKey) },
    { "configUnIndex", "(J)V", (void *) WCDBJNIColumnConstraintFuncName(configUnindexed) },
};

static const JNINativeMethod g_orderingTermMethods[] = {
    { "createCppObj", "(IJ)J", (void *) WCDBJNIOrderingTermFuncName(create) },
    { "collate", "(J" WCDBJNIStringSignature ")V", (void *) WCDBJNIOrderingTermFuncName(configCollation) },
    { "order", "(JI)V", (void *) WCDBJNIOrderingTermFuncName(configOrder) },
};

static const JNINativeMethod g_columnDefMethods[] = {
    { "createCppObj",
      "(" WCDBJNIObjectOrStringSignature "I)J",
      (void *) WCDBJNIColumnDefFuncName(create) },
    { "constraint", "(JJ)V", (void *) WCDBJNIColumnDefFuncName(configConstraint) },
};

static const JNINativeMethod g_indexedColumnMethods[] = {
    { "createCppObj",
      "(" WCDBJNIObjectOrStringSignature ")J",
      (void *) WCDBJNIIndexedColumnFuncName(create) },
    { "collate", "(J" WCDBJNIStringSignature ")V", (void *) WCDBJNIIndexedColumnFuncName(configCollation) },
    { "order", "(JI)V", (void *) WCDBJNIIndexedColumnFuncName(configOrder) },
};

static const JNINativeMethod g_tableOrSubqueryMethods[] = {
    { "createCppObj",
      "(" WCDBJNIObjectOrStringSignature ")J",
      (void *) WCDBJNITableOrSubqueryFuncName(create) },
    { "createCppObj",
      "(" WCDBJNIObjectOrStringArraySignature ")J",
      (void *) WCDBJNITableOrSubqueryFuncName(createWithTableOrSubqueries) },
    { "createWithFunction",
      "(" WCDBJNIStringSignature ")J",
      (void *) WCDBJNITableOrSubqueryFuncName(createWithFunction) },
    { "schema",
      "(J" WCDBJNIObjectOrStringSignature ")V",
      (void *) WCDBJNITableOrSubqueryFuncName(configSchema) },
    { "as", "(J" WCDBJNIStringSignature ")V", (void *) WCDBJNITableOrSubqueryFuncName(configAlias) },
    { "notIndexed", "(J)V", (void *) WCDBJNITableOrSubqueryFuncName(configNotIndexed) },
    { "indexedBy",
      "(J" WCDBJNIStringSignature ")V",
      (void *) WCDBJNITableOrSubqueryFuncName(configIndexName) },
    { "argument", "(J" WCDBJNIMultiTypeSignature ")V", (void *) WCDBJNITableOrSubqueryFuncName(argument) },
};

static const JNINativeMethod g_qualifiedTableMethods[] = {
    { "createCppObj", "(" WCDBJNIStringSignature ")J", (void *) WCDBJNIQualifiedTableFuncName(create) },
    { "schema",
      "(J" WCDBJNIObjectOrStringSignature ")V",
      (void *) WCDBJNIQualifiedTableFuncName(configSchema) },
    { "alias", "(J" WCDBJNIStringSignature ")V", (void *) WCDBJNIQualifiedTableFuncName(configAlias) },
    { "indexed", "(J" WCDBJNIStringSignature ")V", (void *) WCDBJNIQualifiedTableFuncName(configIndex) },
    { "notIndexed", "(J)V", (void *) WCDBJNIQualifiedTableFuncName(configNotIndexed) },
};

static const JNINativeMethod g_resultColumnMethods[] = {
    { "createCppObj",
      "(" WCDBJNIObjectOrStringSignature ")J",
      (void *) WCDBJNIResultColumnFuncName(create) },
    { "configAlias", "(J" WCDBJNIStringSignature ")V", (void *) WCDBJNIResultColumnFuncName(configAlias) },
};

static const JNINativeMethod g_tableConstraintMethods[] = {
    { "createCppObj", "(" WCDBJNIStringSignature ")J", (void *) WCDBJNITableConstraintFuncName(create) },
    { "configPrimaryKey", "(J)V", (void *) WCDBJNITableConstraintFuncName(configPrimaryKey) },
    { "configUnique", "(J)V", (void *) WCDBJNITableConstraintFuncName(configUnique) },
    { "configIndexedColumn",
      "(J" WCDBJNIObjectOrStringArraySignature ")V",
      (void *) WCDBJNITableConstraintFuncName(configIndexedColumn) },
    { "configConfliction", "(JI)V", (void *) WCDBJNITableConstraintFuncName(configConfliction) },
    { "configCheckExpression", "(JJ)V", (void *) WCDBJNITableConstraintFuncName(configCheckCondition) },
    { "configForeignKey",
      "(J" WCDBJNIObjectOrStringArraySignature "J)V",
      (void *) WCDBJNITableConstraintFuncName(configForeignKey) },
};

static const JNINativeMethod g_pragmaMethods[] = {
    { "createCppObj", "(" WCDBJNIStringSignature ")J", (void *) WCDBJNIPragmaFuncName(create) },
};

static const JNINativeMethod g_statementBeginMethods[] = {
    { "createCppObj", "(I)J", (void *) WCDBJNIStatementBeginFuncName(create) },
};

static const JNINativeMethod g_statementCommitMethods[] = {
    { "createCppObj", "()J", (void *) WCDBJNIStatementCommitFuncName(create) },
};

static const JNINativeMethod g_statementCreateIndexMethods[] = {
    { "createCppObj", "()J", (void *) WCDBJNIStatementCreateIndexFuncName(create) },
    { "createIndex",
      "(J" WCDBJNIStringSignature ")V",
      (void *) WCDBJNIStatementCreateIndexFuncName(configIndex) },
    { "configUnique", "(J)V", (void *) WCDBJNIStatementCreateIndexFuncName(configUnique) },
    { "configIfNotExist", "(J)V", (void *) WCDBJNIStatementCreateIndexFuncName(configIfNotExist) },
    { "configSchema",
      "(J" WCDBJNIObjectOrStringSignature ")V",
      (void *) WCDBJNIStatementCreateIndexFuncName(configSchema) },
    { "configTableName",
      "(J" WCDBJNIStringSignature ")V",
      (void *) WCDBJNIStatementCreateIndexFuncName(configTable) },
    { "configIndexedColumns",
      "(J" WCDBJNIObjectOrStringArraySignature ")V",
      (void *) WCDBJNIStatementCreateIndexFuncName(configIndexedColumns) },
    { "configCondition", "(JJ)V", (void *) WCDBJNIStatementCreateIndexFuncName(configWhere) },
};

static const JNINativeMethod g_statementCreateTableMethods[] = {
    { "createCppObj", "()J", (void *) WCDBJNIStatementCreateTableFuncName(create) },
    { "configTableName",
      "(J" WCDBJNIStringSignature ")V",
      (void *) WCDBJNIStatementCreateTableFuncName(configTableName) },
    { "configTemp", "(J)V", (void *) WCDBJNIStatementCreateTableFuncName(configTemp) },
    { "configIfNotExist", "(J)V", (void *) WCDBJNIStatementCreateTableFuncName(configIfNotExist) },
    { "configSchema",
      "(J" WCDBJNIObjectOrStringSignature ")V",
      (void *) WCDBJNIStatementCreateTableFuncName(configSchema) },
    { "configAs", "(JJ)V", (void *) WCDBJNIStatementCreateTableFuncName(configAs) },
    { "configColumn", "(JJ)V", (void *) WCDBJNIStatementCreateTableFuncName(configColumn) },
    { "configColumns", "(J[J)V", (void *) WCDBJNIStatementCreateTableFuncName(configColumns) },
    { "configConstraints", "(J[J)V", (void *) WCDBJNIStatementCreateTableFuncName(configConstraints) },
    { "configWithoutRowid", "(J)V", (void *) WCDBJNIStatementCreateTableFuncName(configWithoutRowid) },
};

static const JNINativeMethod g_statementDeleteMethods[] = {
    { "createCppObj", "()J", (void *) WCDBJNIStatementDeleteFuncName(create) },
    { "configTable",
      "(J" WCDBJNIObjectOrStringSignature ")V",
      (void *) WCDBJNIStatementDeleteFuncName(configTable) },
    { "configCondition", "(JJ)V", (void *) WCDBJNIStatementDeleteFuncName(configCondition) },
    { "configOrders", "(J[J)V", (void *) WCDBJNIStatementDeleteFuncName(configOrders) },
    { "configLimitRange", "(JIJIJ)V", (void *) WCDBJNIStatementDeleteFuncName(configLimitRange) },
    { "configLimitCount", "(JIJ)V", (void *) WCDBJNIStatementDeleteFuncName(configLimitCount) },
    { "configOffset", "(JIJ)V", (void *) WCDBJNIStatementDeleteFuncName(configOffset) },
};

static const JNINativeMethod g_statementDropTableMethods[] = {
    { "createCppObj", "()J", (void *) WCDBJNIStatementDropTableFuncName(create) },
    { "configSchema",
      "(J" WCDBJNIObjectOrStringSignature ")V",
      (void *) WCDBJNIStatementDropTableFuncName(configSchema) },
    { "configTableName",
      "(J" WCDBJNIStringSignature ")V",
      (void *) WCDBJNIStatementDropTableFuncName(configTableName) },
    { "configIfExist", "(J)V", (void *) WCDBJNIStatementDropTableFuncName(configIfExist) },
};

static const JNINativeMethod g_statementInsertMethods[] = {
    { "createCppObj", "()J", (void *) WCDBJNIStatementInsertFuncName(create) },
    { "configSchema",
      "(J" WCDBJNIObjectOrStringSignature ")V",
      (void *) WCDBJNIStatementInsertFuncName(configSchema) },
    { "configTableName",
      "(J" WCDBJNIStringSignature ")V",
      (void *) WCDBJNIStatementInsertFuncName(configTableName) },
    { "configConflictAction", "(JI)V", (void *) WCDBJNIStatementInsertFuncName(configConfliction) },
    { "configAlias", "(J" WCDBJNIStringSignature ")V", (void *) WCDBJNIStatementInsertFuncName(configAs) },
    { "configColumns",
      "(J" WCDBJNIObjectOrStringArraySignature ")V",
      (void *) WCDBJNIStatementInsertFuncName(configColumns) },
    { "configValuesWithBindParameters",
      "(JI)V",
      (void *) WCDBJNIStatementInsertFuncName(configValuesWithBindParameters) },
    { "configValues",
      "(J" WCDBJNIMultiTypeArraySignature ")V",
      (void *) WCDBJNIStatementInsertFuncName(configValues) },
    { "configValues", "(JJ)V", (void *) WCDBJNIStatementInsertFuncName(configSelect) },
};

static const JNINativeMethod g_statementPragmaMethods[] = {
    { "createCppObj", "()J", (void *) WCDBJNIStatementPragmaFuncName(create) },
    { "configSchemaName",
      "(J" WCDBJNIStringSignature ")V",
      (void *) WCDBJNIStatementPragmaFuncName(configSchema) },
    { "configPragma", "(JJ)V", (void *) WCDBJNIStatementPragmaFuncName(configPragma) },
    { "configWithValue",
      "(J" WCDBJNIMultiTypeSignature ")V",
      (void *) WCDBJNIStatementPragmaFuncName(configWithValue) },
    { "configToValue",
      "(J" WCDBJNIMultiTypeSignature ")V",
      (void *) WCDBJNIStatementPragmaFuncName(configToValue) },
};

static const JNINativeMethod g_statementSelectMethods[] = {
    { "createCppObj", "()J", (void *) WCDBJNIStatementSelectFuncName(create) },
    { "configResultColumns",
      "(J" WCDBJNIMultiTypeArraySignature ")V",
      (void *) WCDBJNIStatementSelectFuncName(configResultColumns) },
    { "configDistinct", "(J)V", (void *) WCDBJNIStatementSelectFuncName(configDistiction) },
    { "configTableOrSubqueries",
      "(J" WCDBJNIMultiTypeArraySignature ")V",
      (void *) WCDBJNIStatementSelectFuncName(configTableOrSubqueries) },
    { "configCondition", "(JJ)V", (void *) WCDBJNIStatementSelectFuncName(configCondition) },
    { "configGroups",
      "(J" WCDBJNIMultiTypeArraySignature ")V",
      (void *) WCDBJNIStatementSelectFuncName(configGroups) },
    { "configHaving", "(JJ)V", (void *) WCDBJNIStatementSelectFuncName(configHaving) },
    { "configUnion", "(J)V", (void *) WCDBJNIStatementSelectFuncName(configUnion) },
    { "configUnionAll", "(J)V", (void *) WCDBJNIStatementSelectFuncName(configUnionAll) },
    { "configIntersect", "(J)V", (void *) WCDBJNIStatementSelectFuncName(configIntersect) },
    { "configExcept", "(J)V", (void *) WCDBJNIStatementSelectFuncName(configExcept) },
    { "configOrders", "(J[J)V", (void *) WCDBJNIStatementSelectFuncName(configOrders) },
    { "configLimitRange", "(JIJIJ)V", (void *) WCDBJNIStatementSelectFuncName(configLimitRange) },
    { "configLimitCount", "(JIJ)V", (void *) WCDBJNIStatementSelectFuncName(configLimitCount) },
    { "configOffset", "(JIJ)V", (void *) WCDBJNIStatementSelectFuncName(configOffset) },
};

static const JNINativeMethod g_statementUpdateMethods[] = {
    { "createCppObj", "()J", (void *) WCDBJNIStatementUpdateFuncName(create) },
    { "configTable",
      "(J" WCDBJNIObjectOrStringSignature ")V",
      (void *) WCDBJNIStatementUpdateFuncName(configTable) },
    { "configConflictAction", "(JI)V", (void *) WCDBJNIStatementInsertFuncName(configConfliction) },
    { "configColumnsToBindParameters",
      "(J" WCDBJNIObjectOrStringArraySignature ")V",
      (void *) WCDBJNIStatementUpdateFuncName(configColumnsWithBindParameter) },
    { "configColumnsToValues",
      "(J" WCDBJNIObjectOrStringArraySignature WCDBJNIMultiTypeArraySignature ")V",
      (void *) WCDBJNIStatementUpdateFuncName(configColumnsToValues) },
    { "configColumns",
      "(J" WCDBJNIObjectOrStringArraySignature ")V",
      (void *) WCDBJNIStatementUpdateFuncName(configColumns) },
    { "configToValue",
      "(J" WCDBJNIMultiTypeSignature ")V",
      (void *) WCDBJNIStatementUpdateFuncName(configValue) },
    { "configCondition", "(JJ)V", (void *) WCDBJNIStatementUpdateFuncName(configCondition) },
    { "configOrders", "(J[J)V", (void *) WCDBJNIStatementUpdateFuncName(configOrders) },
    { "configLimitRange", "(JIJIJ)V", (void *) WCDBJNIStatementUpdateFuncName(configLimitRange) },
    { "configLimitCount", "(JIJ)V", (void *) WCDBJNIStatementUpdateFuncName(configLimitCount) },
    { "configOffset", "(JIJ)V", (void *) WCDBJNIStatementUpdateFuncName(configOffset) },
};

static const JNINativeMethod g_tableBindingMethods[] = {
    { "createCppObj", "()J", (void *) WCDBJNIBindingFuncName(create) },
    { "addColumnDef", "(JJ)V", (void *) WCDBJNIBindingFuncName(addColumnDef) },
    { "addIndex", "(J" WCDBJNIStringSignature "J)V", (void *) WCDBJNIBindingFuncName(addIndex) },
    { "addTableConstraint", "(JJ)V", (void *) WCDBJNIBindingFuncName(addTableConstraint) },
    { "configVirtualModule",
      "(J" WCDBJNIStringSignature ")V",
      (void *) WCDBJNIBindingFuncName(configVirtualModule) },
    { "configVirtualModuleArgument",
      "(J" WCDBJNIStringSignature ")V",
      (void *) WCDBJNIBindingFuncName(configVirtualModuleArgument) },
    { "createTable", "(J" WCDBJNIStringSignature "J)Z", (void *) WCDBJNIBindingFuncName(createTable) },
    { "createVirtualTable",
      "(J" WCDBJNIStringSignature "J)Z",
      (void *) WCDBJNIBindingFuncName(createVirtualTable) },
    { "getBaseBinding", "(J)J", (void *) WCDBJNIBindingFuncName(getBaseBinding) },
};

static const JNINativeMethod g_exceptionMethods[] = {
    { "getLevel", "(J)I", (void *) WCDBJNIErrorFuncName(getLevel) },
    { "getCode", "(J)I", (void *) WCDBJNIErrorFuncName(getCode) },
    { "getMessage", "(J)" WCDBJNIStringSignature, (void *) WCDBJNIErrorFuncName(getMessage) },
    { "enumerateInfo", "(J)V", (void *) WCDBJNIErrorFuncName(enumerateInfo) },
};

static const JNINativeMethod g_handleStatementMethods[] = {
    { "getError", "(J)J", (void *) WCDBJNIHandleStatementFuncName(getError) },
    { "prepare", "(JJ)Z", (void *) WCDBJNIHandleStatementFuncName(prepare) },
    { "prepareSQL", "(J" WCDBJNIStringSignature ")Z", (void *) WCDBJNIHandleStatementFuncName(prepareSQL) },
    { "checkPrepared", "(J)Z", (void *) WCDBJNIHandleStatementFuncName(checkPrepared) },
    { "step", "(J)Z", (void *) WCDBJNIHandleStatementFuncName(step) },
    { "reset", "(J)V", (void *) WCDBJNIHandleStatementFuncName(reset) },
    { "finalize", "(J)V", (void *) WCDBJNIHandleStatementFuncName(finalize) },
    { "isDone", "(J)Z", (void *) WCDBJNIHandleStatementFuncName(isDone) },
    { "bindInteger", "(JJI)V", (void *) WCDBJNIHandleStatementFuncName(bindInteger) },
    { "bindDouble", "(JDI)V", (void *) WCDBJNIHandleStatementFuncName(bindDouble) },
    { "bindText", "(J" WCDBJNIStringSignature "I)V", (void *) WCDBJNIHandleStatementFuncName(bindText) },
    { "bindBLOB", "(J[BI)V", (void *) WCDBJNIHandleStatementFuncName(bindBLOB) },
    { "bindNull", "(JI)V", (void *) WCDBJNIHandleStatementFuncName(bindNull) },
    { "getColumnType", "(JI)I", (void *) WCDBJNIHandleStatementFuncName(getColumnType) },
    { "getInteger", "(JI)J", (void *) WCDBJNIHandleStatementFuncName(getInteger) },
    { "getDouble", "(JI)D", (void *) WCDBJNIHandleStatementFuncName(getDouble) },
    { "getText", "(JI)" WCDBJNIStringSignature, (void *) WCDBJNIHandleStatementFuncName(getText) },
    { "getBLOB", "(JI)[B", (void *) WCDBJNIHandleStatementFuncName(getBLOB) },
    { "getColumnCount", "(J)I", (void *) WCDBJNIHandleStatementFuncName(getColumnCount) },
    { "bindParameterIndex",
      "(J" WCDBJNIStringSignature ")I",
      (void *) WCDBJNIHandleStatementFuncName(bindParameterIndex) },
    { "getColumnName",
      "(JI)" WCDBJNIStringSignature,
      (void *) WCDBJNIHandleStatementFuncName(getColumnName) },
    { "getOriginalColumnName",
      "(JI)" WCDBJNIStringSignature,
      (void *) WCDBJNIHandleStatementFuncName(getOriginalColumnName) },
    { "getColumnTableName",
      "(JI)" WCDBJNIStringSignature,
      (void *) WCDBJNIHandleStatementFuncName(getColumnTableName) },
    { "getColumnName",
      "(JI)" WCDBJNIStringSignature,
      (void *) WCDBJNIHandleStatementFuncName(getColumnName) },
    { "isReadOnly", "(J)Z", (void *) WCDBJNIHandleStatementFuncName(isReadOnly) },
};

static const JNINativeMethod g_handleMethods[] = {
    { "getError", "(J)J", (void *) WCDBJNIHandleFuncName(getError) },
    { "checkValid", "(J)Z", (void *) WCDBJNIHandleFuncName(checkValid) },
    { "getOrCreatePreparedStatement", "(JJ)J", (void *) WCDBJNIHandleFuncName(getOrCreatePreparedStatement) },
    { "getOrCreatePreparedStatementWithSQL",
      "(J" WCDBJNIStringSignature ")J",
      (void *) WCDBJNIHandleFuncName(getOrCreatePreparedStatementWithSQL) },
    { "getMainStatement", "(J)J", (void *) WCDBJNIHandleFuncName(getMainStatement) },
    { "finalizeAllStatements", "(J)V", (void *) WCDBJNIHandleFuncName(finalizeAllStatements) },
    { "execute", "(JJ)Z", (void *) WCDBJNIHandleFuncName(execute) },
    { "executeSQL", "(J" WCDBJNIStringSignature ")Z", (void *) WCDBJNIHandleFuncName(executeSQL) },
    { "tableExist", "(J" WCDBJNIStringSignature ")I", (void *) WCDBJNIHandleFuncName(tableExist) },
    { "getChanges", "(J)I", (void *) WCDBJNIHandleFuncName(getChanges) },
    { "getTotalChanges", "(J)I", (void *) WCDBJNIHandleFuncName(getTotalChanges) },
    { "getLastInsertedRowId", "(J)J", (void *) WCDBJNIHandleFuncName(getLastInsertRowid) },
    { "isInTransaction", "(J)Z", (void *) WCDBJNIHandleFuncName(isInTransaction) },
    { "beginTransaction", "(J)Z", (void *) WCDBJNIHandleFuncName(beginTransaction) },
    { "commitTransaction", "(J)Z", (void *) WCDBJNIHandleFuncName(commitTransaction) },
    { "rollbackTransaction", "(J)V", (void *) WCDBJNIHandleFuncName(rollbackTransaction) },
    { "beginNestedTransaction", "(J)Z", (void *) WCDBJNIHandleFuncName(beginNestedTransaction) },
    { "commitNestedTransaction", "(J)Z", (void *) WCDBJNIHandleFuncName(commitNestedTransaction) },
    { "rollbackNestedTransaction", "(J)V", (void *) WCDBJNIHandleFuncName(rollbackNestedTransaction) },
    { "runTransaction",
      "(JLcom/tencent/wcdb/core/HandleOperation$Transaction;)Z",
      (void *) WCDBJNIHandleFuncName(runTransaction) },
    { "runNestedTransaction",
      "(JLcom/tencent/wcdb/core/HandleOperation$Transaction;)Z",
      (void *) WCDBJNIHandleFuncName(runNestedTransaction) },
    { "runPausableTransaction",
      "(JLcom/tencent/wcdb/core/HandleOperation$PausableTransaction;)Z",
      (void *) WCDBJNIHandleFuncName(runPausableTransaction) },
};

static const JNINativeMethod g_databaseMethods[]
= { { "createDatabase", "(" WCDBJNIStringSignature ")J", (void *) WCDBJNICoreFuncName(createDatabase) },
    { "setDefaultCipherVersion", "(I)V", (void *) WCDBJNICoreFuncName(setDefaultCipherConfig) },
    { "purgeAll", "()V", (void *) WCDBJNICoreFuncName(purgeAllDatabase) },
    { "getError", "(J)J", (void *) WCDBJNIDatabaseFuncName(getError) },
    { "getPath", "(J)" WCDBJNIStringSignature, (void *) WCDBJNIDatabaseFuncName(getPath) },
    { "getPaths", "(J)Ljava/util/ArrayList;", (void *) WCDBJNIDatabaseFuncName(getPaths) },
    { "setTag", "(JJ)V", (void *) WCDBJNIDatabaseFuncName(setTag) },
    { "getTag", "(J)J", (void *) WCDBJNIDatabaseFuncName(getTag) },
    { "canOpen", "(J)Z", (void *) WCDBJNIDatabaseFuncName(canOpen) },
    { "isOpened", "(J)Z", (void *) WCDBJNIDatabaseFuncName(isOpened) },
    { "close",
      "(J" WCDBJNIDatabaseSignature "$CloseCallBack;)V",
      (void *) WCDBJNIDatabaseFuncName(close) },
    { "blockade", "(J)V", (void *) WCDBJNIDatabaseFuncName(blockade) },
    { "unblockade", "(J)V", (void *) WCDBJNIDatabaseFuncName(unblockade) },
    { "isBlockaded", "(J)Z", (void *) WCDBJNIDatabaseFuncName(isBlockaded) },
    { "purge", "(J)V", (void *) WCDBJNIDatabaseFuncName(purge) },
    { "getHandle", "(J)J", (void *) WCDBJNIDatabaseFuncName(getHandle) },
    { "setCipherKey", "(J[BII)V", (void *) WCDBJNIDatabaseFuncName(configCipher) },
    { "setConfig",
      "(J" WCDBJNIStringSignature WCDBJNIDatabaseSignature
      "$Config;" WCDBJNIDatabaseSignature "$Config;I)V",
      (void *) WCDBJNIDatabaseFuncName(config) },
    { "globalTracePerformance",
      "(" WCDBJNIDatabaseSignature "$PerformanceTracer;)V",
      (void *) WCDBJNIDatabaseFuncName(globalTracePerformance) },
    { "tracePerformance",
      "(J" WCDBJNIDatabaseSignature "$PerformanceTracer;)V",
      (void *) WCDBJNIDatabaseFuncName(tracePerformance) },
    { "globalTraceSQL",
      "(" WCDBJNIDatabaseSignature "$SQLTracer;)V",
      (void *) WCDBJNIDatabaseFuncName(globalTraceSQL) },
    { "traceSQL",
      "(J" WCDBJNIDatabaseSignature "$SQLTracer;)V",
      (void *) WCDBJNIDatabaseFuncName(traceSQL) },
    { "globalTraceException",
      "(" WCDBJNIDatabaseSignature "$ExceptionTracer;)V",
      (void *) WCDBJNIDatabaseFuncName(globalTraceError) },
    { "traceException",
      "(J" WCDBJNIDatabaseSignature "$ExceptionTracer;)V",
      (void *) WCDBJNIDatabaseFuncName(traceError) },
    { "globalTraceDatabaseOperation",
      "(" WCDBJNIDatabaseSignature "$OperationTracer;)V",
      (void *) WCDBJNIDatabaseFuncName(globalTraceOperation) },
    { "removeFiles", "(J)Z", (void *) WCDBJNIDatabaseFuncName(removeFiles) },
    { "moveFile", "(J" WCDBJNIStringSignature ")Z", (void *) WCDBJNIDatabaseFuncName(moveFile) },
    { "getFileSize", "(J)J", (void *) WCDBJNIDatabaseFuncName(getFileSize) },
    { "setNotificationWhenCorrupted",
      "(J" WCDBJNIDatabaseSignature "$CorruptionNotification;)V",
      (void *) WCDBJNIDatabaseFuncName(setNotificationWhenCorrupted) },
    { "checkIfCorrupted", "(J)Z", (void *) WCDBJNIDatabaseFuncName(checkIfCorrupted) },
    { "checkIfIsAlreadyCorrupted", "(J)Z", (void *) WCDBJNIDatabaseFuncName(checkIfIsAlreadyCorrupted) },
    { "enableAutoBackup", "(JZ)V", (void *) WCDBJNIDatabaseFuncName(enableAutoBackup) },
    { "backup", "(J)Z", (void *) WCDBJNIDatabaseFuncName(backup) },
    { "filterBackup",
      "(J" WCDBJNIDatabaseSignature "$BackupFilter;)V",
      (void *) WCDBJNIDatabaseFuncName(filterBackup) },
    { "retrieve",
      "(J" WCDBJNIDatabaseSignature "$RetrieveProgressMonitor;)D",
      (void *) WCDBJNIDatabaseFuncName(retrieve) },
    { "deposit", "(J)Z", (void *) WCDBJNIDatabaseFuncName(deposit) },
    { "removeDepositedFiles", "(J)Z", (void *) WCDBJNIDatabaseFuncName(removeDepositedFiles) },
    { "containDepositedFiles", "(J)Z", (void *) WCDBJNIDatabaseFuncName(containDepositedFiles) },
    { "passiveCheckpoint", "(J)Z", (void *) WCDBJNIDatabaseFuncName(passiveCheckpoint) },
    { "truncateCheckpoint", "(J)Z", (void *) WCDBJNIDatabaseFuncName(truncateCheckpoint) },
    { "filterMigration",
      "(J" WCDBJNIDatabaseSignature "$MigrationFilter;)V",
      (void *) WCDBJNIDatabaseFuncName(filterMigration) },
    { "stepMigration", "(J)Z", (void *) WCDBJNIDatabaseFuncName(stepMigration) },
    { "enableAutoMigration", "(JZ)V", (void *) WCDBJNIDatabaseFuncName(enableAutoMigration) },
    { "setNotificationWhenMigrated",
      "(J" WCDBJNIDatabaseSignature "$MigrationNotification;)V",
      (void *) WCDBJNIDatabaseFuncName(setNotificationWhenMigrated) },
    { "isMigrated", "(J)Z", (void *) WCDBJNIDatabaseFuncName(isMigrated) } };

static const JNIBinding g_bindingInfo[] = {
    WCDBJNIRegister("com/tencent/wcdb/base/CppObject", g_objectBridgeMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/Identifier", g_winqBridgeMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/LiteralValue", g_literalValueMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/Schema", g_schemaMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/Column", g_columnMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/BindParameter", g_bindParameterMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/Expression", g_expressionMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/ExpressionOperable", g_expressionOperationMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/ColumnConstraint", g_columnConstraintMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/OrderingTerm", g_orderingTermMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/ColumnDef", g_columnDefMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/IndexedColumn", g_indexedColumnMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/TableOrSubquery", g_tableOrSubqueryMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/QualifiedTable", g_qualifiedTableMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/ResultColumn", g_resultColumnMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/TableConstraint", g_tableConstraintMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/Pragma", g_pragmaMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/StatementBegin", g_statementBeginMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/StatementCommit", g_statementCommitMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/StatementCreateIndex", g_statementCreateIndexMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/StatementCreateTable", g_statementCreateTableMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/StatementDelete", g_statementDeleteMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/StatementDropTable", g_statementDropTableMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/StatementInsert", g_statementInsertMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/StatementPragma", g_statementPragmaMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/StatementSelect", g_statementSelectMethods),
    WCDBJNIRegister("com/tencent/wcdb/winq/StatementUpdate", g_statementUpdateMethods),
    WCDBJNIRegister("com/tencent/wcdb/orm/Binding", g_tableBindingMethods),
    WCDBJNIRegister("com/tencent/wcdb/base/WCDBException", g_exceptionMethods),
    WCDBJNIRegister("com/tencent/wcdb/core/PreparedStatement", g_handleStatementMethods),
    WCDBJNIRegister("com/tencent/wcdb/core/Handle", g_handleMethods),
    WCDBJNIRegister("com/tencent/wcdb/core/Database", g_databaseMethods),
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *unused)
{
    JNIEnv *env = NULL;
    jint result = JNI_ERR;

    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }

    for (int i = 0; i < sizeof(g_bindingInfo) / sizeof(JNIBinding); i++) {
        jclass clazz = (*env)->FindClass(env, g_bindingInfo[i].className);
        if (clazz == NULL) {
            (*env)->DeleteLocalRef(env, clazz);
            return result;
        }
        if ((*env)->RegisterNatives(
            env, clazz, g_bindingInfo[i].methods, g_bindingInfo[i].methodCount)
            < 0) {
            return result;
        }
        (*env)->DeleteLocalRef(env, clazz);
    }
    result = JNI_VERSION_1_6;
    return result;
}

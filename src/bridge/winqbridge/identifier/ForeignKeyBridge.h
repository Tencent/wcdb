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

#pragma once

#include "WinqBridge.h"

WCDB_EXTERN_C_BEGIN

CPPForeignKey WCDBForeignKeyCreate(void);
void WCDBForeignKeyConfigReferencesTable(CPPForeignKey foreignKey, const char* _Nullable table);
void WCDBForeignKeyAddColumn(CPPForeignKey foreignKey, CPPColumn column);
void WCDBForeignKeyAddColumn2(CPPForeignKey foreignKey, CPPCommonArray columns);

void WCDBForeignKeyConfigOnDeleteSetNull(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigOnDeleteSetDefault(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigOnDeleteCascade(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigOnDeleteRestrict(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigOnDeleteNoAction(CPPForeignKey foreignKey);

void WCDBForeignKeyConfigOnUpdateSetNull(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigOnUpdateSetDefault(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigOnUpdateCascade(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigOnUpdateRestrict(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigOnUpdateNoAction(CPPForeignKey foreignKey);

void WCDBForeignKeyConfigMatch(CPPForeignKey foreignKey, enum WCDBSyntaxMatchType match);

void WCDBForeignKeyConfigDeferrable(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigDeferrableInitiallyDeferred(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigDeferrableInitiallyImmediate(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigNotDeferrable(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigNotDeferrableInitiallyDeferred(CPPForeignKey foreignKey);
void WCDBForeignKeyConfigNotDeferrableInitiallyImmediate(CPPForeignKey foreignKey);

WCDB_EXTERN_C_END

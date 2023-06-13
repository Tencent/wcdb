//
// Created by qiuwenchen on 2022/6/12.
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

#include "WinqBridge.h"

WCDB_EXTERN_C_BEGIN

CPPUpsert WCDBUpsertCreate(void);
void WCDBUpsertConfigIndexdColumn(CPPUpsert upsert,
                                  const CPPIndexedColumn* _Nullable columns,
                                  int colnum);
void WCDBUpsertConfigIndexdColumn2(CPPUpsert upsert, CPPCommonArray columns);
void WCDBUpsertConfigWhere(CPPUpsert upsert, CPPExpression expression);
void WCDBUpsertConfigDoNothing(CPPUpsert upsert);
void WCDBUpsertConfigDoUpdate(CPPUpsert upsert);
void WCDBUpsertConfigSetColumns(CPPUpsert upsert, const CPPColumn* _Nullable columns, int colNum);
void WCDBUpsertConfigSetColumns2(CPPUpsert upsert, CPPCommonArray columns);
void WCDBUpsertConfigToValue(CPPUpsert upsert, CPPExpression expression);
void WCDBUpsertConfigToValue2(CPPUpsert upsert, CPPCommonValue expression);

WCDB_EXTERN_C_END

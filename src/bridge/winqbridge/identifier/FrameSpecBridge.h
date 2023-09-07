//
// Created by qiuwenchen on 2022/6/9.
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

CPPFrameSpec WCDBFrameSpecCreate(void);
void WCDBFrameSpecConfigRange(CPPFrameSpec frameSpec);
void WCDBFrameSpecConfigRows(CPPFrameSpec frameSpec);
void WCDBFrameSpecConfigUnboundedPreceding(CPPFrameSpec frameSpec);
void WCDBFrameSpecConfigPreceding(CPPFrameSpec frameSpec, CPPExpression expression);
void WCDBFrameSpecConfigPreceding2(CPPFrameSpec frameSpec, CPPCommonValue expression);
void WCDBFrameSpecConfigCurrentRow(CPPFrameSpec frameSpec);
void WCDBFrameSpecConfigBetweenUnboundedPreceding(CPPFrameSpec frameSpec);
void WCDBFrameSpecConfigBetweenPreceding(CPPFrameSpec frameSpec, CPPExpression expression);
void WCDBFrameSpecConfigBetweenPreceding2(CPPFrameSpec frameSpec, CPPCommonValue expression);
void WCDBFrameSpecConfigBetweenCurrentRow(CPPFrameSpec frameSpec);
void WCDBFrameSpecConfigBetweenFollowing(CPPFrameSpec frameSpec, CPPExpression expression);
void WCDBFrameSpecConfigBetweenFollowing2(CPPFrameSpec frameSpec, CPPCommonValue expression);
void WCDBFrameSpecConfigAndPreceding(CPPFrameSpec frameSpec, CPPExpression expression);
void WCDBFrameSpecConfigAndPreceding2(CPPFrameSpec frameSpec, CPPCommonValue expression);
void WCDBFrameSpecConfigAndCurrentRow(CPPFrameSpec frameSpec);
void WCDBFrameSpecConfigAndFollowing(CPPFrameSpec frameSpec, CPPExpression expression);
void WCDBFrameSpecConfigAndFollowing2(CPPFrameSpec frameSpec, CPPCommonValue expression);
void WCDBFrameSpecConfigAndUnboundedFollowing(CPPFrameSpec frameSpec);

WCDB_EXTERN_C_END

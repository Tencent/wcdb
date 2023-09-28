//
// Created by qiuwenchen on 2022/4/1.
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

#include "ContainerBridge.h"
#include "ObjectBridge.h"

WCDB_EXTERN_C_BEGIN

WCDBDefineCPPBridgedType(CPPError)

enum WCDBErrorLevel {
    WCDBErrorLevel_Ignore = 1,
    WCDBErrorLevel_Debug,
    WCDBErrorLevel_Notice,
    WCDBErrorLevel_Warning,
    WCDBErrorLevel_Error,
    WCDBErrorLevel_Fatal,
};

long WCDBErrorGetLevel(CPPError obj);
long WCDBErrorGetCode(CPPError obj);
const char* _Nonnull WCDBErrorGetMsg(CPPError obj);

void WCDBErrorEnumerateAllInfo(CPPError error,
                               void* _Nonnull context,
                               StringViewMapEnumerator _Nonnull callback);

void WCDBErrorReport(enum WCDBErrorLevel level,
                     long code,
                     const char* _Nullable content,
                     const char* _Nullable path,
                     long tag);

WCDB_EXTERN_C_END

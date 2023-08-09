//
// Created by 陈秋文 on 2023/8/6.
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

#include "ObjectBridge.h"

WCDB_EXTERN_C_BEGIN

typedef void (*StringViewMapEnumerator)(void* _Nonnull context,
                                        const char* _Nonnull key,
                                        CPPCommonValue value);
void WCDBEnumerateStringViewMap(const void* _Nonnull map,
                                void* _Nonnull context,
                                StringViewMapEnumerator _Nonnull enumerator);

WCDB_EXTERN_C_END

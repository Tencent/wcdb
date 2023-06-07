//
// Created by qiuwenchen on 2023/6/6.
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
#include <sys/types.h>
#ifdef _WIN32
#include <BaseTsd.h>
#endif

namespace WCDB {

#ifndef _WIN32

typedef off_t offset_t;

#else

typedef SSIZE_T ssize_t;
typedef SSIZE_T offset_t;

#endif //_WIN32

} //namespace WCDB

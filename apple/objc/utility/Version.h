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

#define WCDB_IDENTIFIER "8e3f0e2f9c9b06619b166bf3e2897b46"

#ifdef WCDB_VERSION
#define WCDB_VERSION_STRING WCDB_STRINGIFY(WCDB_VERSION)
#else
#define WCDB_VERSION_STRING "Custom build"
#endif

#ifdef WCDB_BUILD
#define WCDB_BUILD_STRING WCDB_STRINGIFY(WCDB_BUILD)
#else
#define WCDB_BUILD_STRING "Custom Build"
#endif

#define WCDB_SOURCE_ID                                                         \
    WCDB_IDENTIFIER ", " WCDB_VERSION_STRING "." WCDB_BUILD_STRING

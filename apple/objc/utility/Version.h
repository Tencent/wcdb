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

/* This file is generated automatically by `tools/version/bump.sh`. */

#define WCDB_IDENTIFIER "bfd56a1a2d98b0b21babd84fcf6db4ac"

#ifdef WCDB_TIMESTAMP
#define WCDB_TIMESTAMP_STRING WCDB_STRINGIFY(WCDB_TIMESTAMP)
#else
#define WCDB_TIMESTAMP_STRING "Undefine Timestamp"
#endif

#ifdef WCDB_VERSION
#define WCDB_VERSION_STRING WCDB_STRINGIFY(WCDB_VERSION)
#else
#define WCDB_VERSION_STRING "Undefined Version"
#endif

#ifdef WCDB_BUILD
#define WCDB_BUILD_STRING WCDB_STRINGIFY(WCDB_BUILD)
#else
#define WCDB_BUILD_STRING "Undefined Build"
#endif

#define WCDB_SOURCE_ID                                                         \
    WCDB_IDENTIFIER ", " WCDB_VERSION_STRING "." WCDB_BUILD_STRING ", " WCDB_TIMESTAMP_STRING

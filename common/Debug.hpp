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

#ifndef __WCDB_DEBUG_HPP
#define __WCDB_DEBUG_HPP

#ifndef WCDB_DEBUG
#if DEBUG
#define WCDB_DEBUG 1
#else // DEBUG
#define WCDB_DEBUG 0
#endif // DEBUG
#endif // WCDB_DEBUG

#if WCDB_DEBUG

#define WCDB_TRACE 1

#else // WCDB_DEBUG

#define WCDB_TRACE 0

#endif // WCDB_DEBUG

#define WCDB_ASSERTION WCDB_TRACE

#endif /* __WCDB_DEBUG_HPP */

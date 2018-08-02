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

#ifndef GlobalConfig_hpp
#define GlobalConfig_hpp

#include <functional>

#pragma GCC visibility push(hidden)

namespace WCDB {

class GlobalConfig {
public:
    static void enableMultithread();
    static void enableMemoryStatus(bool enable);
    static void setMemoryMapSize(int64_t defaultSizeLimit, int64_t maximumAllowedSizeLimit);

    typedef void (*Log)(void *, int, const char *);
    static void setNotificationForLog(const Log &log);

    typedef int (*VFSOpen)(const char *, int, int);
    static void hookVFSOpen(const VFSOpen &vfsOpen);
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* GlobalConfig_hpp */

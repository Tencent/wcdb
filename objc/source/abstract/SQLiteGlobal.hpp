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

#ifndef SQLiteGlobal_hpp
#define SQLiteGlobal_hpp

#include <functional>

#pragma GCC visibility push(hidden)

namespace WCDB {

class SQLiteGlobal {
public:
    static SQLiteGlobal *shared();

    void setNotificationWhenFileCreated(
        const std::function<void(const char *)> &onFileCreated);

protected:
    SQLiteGlobal();
    SQLiteGlobal(const SQLiteGlobal &) = delete;
    SQLiteGlobal &operator=(const SQLiteGlobal &) = delete;

    static int vfsOpen(const char *zFile, int flags, int mode);
    static void log(void *userInfo, int code, const char *message);

    std::function<void(const char *)> m_onFileCreated;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* SQLiteGlobal_hpp */

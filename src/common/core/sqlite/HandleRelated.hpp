//
// Created by sanhuazhang on 2019/05/19
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

#include "SQLiteDeclaration.h"
#include "StringView.hpp"

namespace WCDB {

class AbstractHandle;

class HandleRelated {
public:
    HandleRelated(AbstractHandle *handle);
    virtual ~HandleRelated() = 0;

    AbstractHandle *getHandle() const;
    sqlite3 *getRawHandle();

protected:
    bool APIExit(int rc);
    bool APIExit(int rc, const UnsafeStringView &sql);
    bool APIExit(int rc, const char *sql);
    void cacheCurrentTransactionError();
    void resumeCacheTransactionError();
    bool isBusyTraceEnable() const;
    void setCurrentSQL(const UnsafeStringView &sql);
    void resetCurrentSQL(const UnsafeStringView &sql);

private:
    AbstractHandle *m_handle;
};

} //namespace WCDB

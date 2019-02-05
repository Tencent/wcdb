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

#ifndef __WCDB_HANDLE_RELATED_HPP
#define __WCDB_HANDLE_RELATED_HPP

#include <WCDB/SQLiteDeclaration.h>
#include <WCDB/String.hpp>

namespace WCDB {

class AbstractHandle;
class HandleStatement;

class HandleRelated {
public:
    HandleRelated(AbstractHandle *handle);
    virtual ~HandleRelated() = 0;

    AbstractHandle *getHandle() const;

protected:
    bool exitAPI(int rc);
    bool exitAPI(int rc, const String &sql);
    bool exitAPI(int rc, const char *sql);
    AbstractHandle *m_handle;
};

class HandleStatementEvent {
public:
    virtual ~HandleStatementEvent() = 0;

    virtual void statementWillStep(HandleStatement *) = 0;
    virtual void statementDidStep(HandleStatement *, bool) = 0;
};

} //namespace WCDB

#endif /* __WCDB_HANDLE_RELATED_HPP */

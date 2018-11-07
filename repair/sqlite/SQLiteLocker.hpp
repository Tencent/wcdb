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

#ifndef _WCDB_SQLITELOCKER_HPP
#define _WCDB_SQLITELOCKER_HPP

#include <WCDB/Locker.hpp>
#include <WCDB/SQLiteBase.hpp>

namespace WCDB {

namespace Repair {

class SQLiteReadLocker final : public ReadLocker, public SQLiteBase {
public:
    void setPath(const String &path) override final;
    const String &getPath() const override final;

    bool acquireLock() override final;
    bool releaseLock() override final;
    const Error &getError() const override final;
};

class SQLiteWriteLocker final : public WriteLocker, public SQLiteBase {
public:
    void setPath(const String &path) override final;
    const String &getPath() const override final;

    bool acquireLock() override final;
    bool releaseLock() override final;
    const Error &getError() const override final;
};

} //namespace Repair

} //namespace WCDB

#endif /* _WCDB_SQLITELOCKER_HPP */

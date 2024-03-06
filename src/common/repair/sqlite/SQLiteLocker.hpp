//
// Created by sanhuazhang on 2018/06/22
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

#if WCDB_USE_BUILTIN_SQLITE_REPAIR

#include "SQLiteBase.hpp"

namespace WCDB {

namespace Repair {

class SQLiteReadLocker final : public ReadLocker, public SQLiteBase {
public:
    ~SQLiteReadLocker() override;

    void setPath(const UnsafeStringView &path) override final;
    const StringView &getPath() const override final;

    bool acquireLock() override final;
    bool releaseLock() override final;
    const Error &getError() const override final;
};

class SQLiteWriteLocker final : public WriteLocker, public SQLiteBase {
public:
    ~SQLiteWriteLocker() override;
    void setPath(const UnsafeStringView &path) override final;
    const StringView &getPath() const override final;

    bool acquireLock() override final;
    bool releaseLock() override final;
    const Error &getError() const override final;
};

} //namespace Repair

} //namespace WCDB

#endif /* WCDB_USE_BUILTIN_SQLITE_REPAIR */

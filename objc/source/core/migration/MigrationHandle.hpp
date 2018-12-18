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

#ifndef _WCDB_MIGRATIONHANDLE_HPP
#define _WCDB_MIGRATIONHANDLE_HPP

#include <WCDB/Configs.hpp>
#include <WCDB/Migration.hpp>

namespace WCDB {

class MigrationInfo;

class MigrationHandle final : public ConfigurableHandle, public Migration::Binder {
#pragma mark - Initialize
public:
    MigrationHandle(Migration &migration);

#pragma mark - Bind
protected:
    bool rebind(const std::set<const MigrationInfo *> &toRebinds) override final;
    std::pair<bool, std::set<String>>
    getColumns(const String &table, const String &database) override final;

#pragma mark - Migration
public:
protected:
    std::pair<bool, bool> tamper(Statement &statement);
    bool preprocess(Statement &statement);

#pragma mark - Override
public:
    bool execute(const Statement &statement) override final;
    bool prepare(const Statement &statement) override final;
    bool step(bool &done) override final;
    void reset() override final;
    void finalize() override final;
    void bindInteger32(const Integer32 &value, int index) override final;
    void bindInteger64(const Integer64 &value, int index) override final;
    void bindDouble(const Float &value, int index) override final;
    void bindText(const Text &value, int index) override final;
    void bindBLOB(const BLOB &value, int index) override final;
    void bindNull(int index) override final;
};

} //namespace WCDB

#endif /* _WCDB_MIGRATIONHANDLE_HPP */

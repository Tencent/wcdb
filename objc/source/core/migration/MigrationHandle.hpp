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

#ifndef MigrationHandle_hpp
#define MigrationHandle_hpp

#include <WCDB/Abstract.h>
#include <WCDB/MigrationSetting.hpp>
#include <WCDB/MigrationTamperer.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class MigrationHandle : public Handle {
#pragma mark - Initialize
public:
    static std::shared_ptr<Handle>
    handleWithPath(const std::string &path, MigrationSetting &setting);

protected:
    MigrationHandle(const std::string &path, MigrationSetting &setting);

#pragma mark - Override
public:
    bool execute(const Statement &statement) override;
    bool prepare(const Statement &statement) override;
    bool step(bool &done) override;
    void reset() override;
    using Handle::step;
    void finalize() override;
    void bindInteger32(const Integer32 &value, int index) override;
    void bindInteger64(const Integer64 &value, int index) override;
    void bindDouble(const Float &value, int index) override;
    void bindText(const Text &value, int length, int index) override;
    void bindBLOB(const BLOB &value, int index) override;
    void bindNull(int index) override;

#pragma mark - Migration
public:
    bool migrateWithRowID(const long long &rowid,
                          const std::shared_ptr<MigrationInfo> &info,
                          const Lang::InsertSTMT::Type &onConflict);

protected:
    bool executeWithoutTampering(const Statement &statement);
    bool prepareWithoutTampering(const Statement &statement);

#ifdef DEBUG
    void debug_checkStatementLegal(const Statement &statement);
#endif
    MigrationSetting &m_setting;

#pragma mark - Multiple Statements
protected:
    bool prepareWithMultipleStatements(const Statement &statement,
                                       const Statement &tamperedStatement);
    bool _migrateWithRowID(const long long &rowid, HandleStatement &handleStatement);

    HandleStatement m_tamperedHandleStatement;
    bool m_unlockShared;

    HandleStatement m_extraHandleStatement1;
    HandleStatement m_extraHandleStatement2;
    Lang::InsertSTMT::Type m_associatedConflictType;

    MigrationTamperer m_tamperer;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* MigrationHandle_hpp */

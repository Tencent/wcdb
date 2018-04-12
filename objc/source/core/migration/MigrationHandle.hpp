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
#include <WCDB/MigrationInfos.hpp>

namespace WCDB {

class MigrationHandle : public Handle {
#pragma mark - Initialize
public:
    static std::shared_ptr<Handle>
    handleWithPath(const std::string &path,
                   Tag tag,
                   const std::shared_ptr<MigrationInfos> &infos);

protected:
    MigrationHandle(const std::string &path,
                    Tag tag,
                    const std::shared_ptr<MigrationInfos> &infos);

#pragma mark - Override
public:
    virtual bool execute(const Statement &statement) override;
    virtual bool prepare(const Statement &statement) override;
    virtual bool step(bool &done) override;
    void reset() override;
    using Handle::step;
    virtual void finalize() override;
    virtual void bindInteger32(const Integer32 &value, int index) override;
    virtual void bindInteger64(const Integer64 &value, int index) override;
    virtual void bindDouble(const Float &value, int index) override;
    virtual void bindText(const Text &value, int length, int index) override;
    virtual void bindBLOB(const BLOB &value, int index) override;
    virtual void bindNull(int index) override;

#pragma mark - Migration
public:
    bool lazySetupVeryFirstMigratingInfo();
    bool executeWithoutTampering(const Statement &statement);
    bool prepareWithoutTampering(const Statement &statement);

protected:
#ifdef DEBUG
    void debugCheckStatementLegal(const Statement &statement);
#endif
    std::shared_ptr<MigrationInfos> m_infos;

#pragma mark - Multiple Statements
protected:
    bool executeWithMultipleStatements(const Statement &statement,
                                       const Statement &tamperedStatement);
    bool prepareWithMultipleStatements(const Statement &statement,
                                       const Statement &tamperedStatement);

    HandleStatement m_tamperedHandleStatement;
};

} //namespace WCDB

#endif /* MigrationHandle_hpp */

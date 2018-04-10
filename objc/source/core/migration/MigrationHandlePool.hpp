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

#ifndef MigrationHandlePool_hpp
#define MigrationHandlePool_hpp

#include <WCDB/HandlePool.hpp>
#include <WCDB/MigrationInfo.hpp>

namespace WCDB {

class MigrationHandlePool : public HandlePool {
#pragma mark - Initialize
public:
    friend class MigrationHandlePools;
    MigrationHandlePool(const std::string &path, const Configs &configs);

#pragma mark - Migration
    //They are not thread-safe except `isMigrating`.
public:
    void setMigrationInfo(const std::shared_ptr<MigrationInfo> &migrationInfo);
    MigrationInfo *getMigrationInfo() const;

protected:
    std::shared_ptr<MigrationInfo> m_info;

#pragma mark - Override
public:
    virtual RecyclableHandle flowOut() override;

protected:
    virtual std::shared_ptr<Handle> generateHandle() override;
};

} //namespace WCDB

#endif /* MigrationHandlePool_hpp */

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
#include <WCDB/MigrationInfos.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class MigrationHandlePool : public HandlePool {
#pragma mark - Initialize
public:
    friend class MigrationHandlePools;
    MigrationHandlePool(const std::string &path,
                        const Configs &configs,
                        const std::shared_ptr<MigrationInfos> &migrationInfos);

#ifdef DEBUG
public:
    bool debug_checkMigratingThread();

protected:
    std::atomic<std::thread::id> debug_m_migratingThreadId;
    bool debug_checkInfosLegal();
#endif

#pragma mark - Migration
public:
    MigrationInfos *getMigrationInfos() const;

protected:
    std::shared_ptr<MigrationInfos> m_infos;

#pragma mark - Override
protected:
    bool willConfigurateHandle(Handle *handle) override;
    std::shared_ptr<Handle> generateHandle() override;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* MigrationHandlePool_hpp */

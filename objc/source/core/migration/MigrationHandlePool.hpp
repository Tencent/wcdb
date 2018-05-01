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
#include <WCDB/MigrationSetting.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class MigrationHandlePool : public HandlePool {
#pragma mark - Initialize
public:
    static std::shared_ptr<HandlePool>
    pool(const std::string &path,
         const Configs &configs,
         const std::list<std::shared_ptr<MigrationInfo>> &infos);

protected:
    MigrationHandlePool(const std::string &path,
                        const Configs &configs,
                        const std::list<std::shared_ptr<MigrationInfo>> &infos);
    void initialize();

#ifdef DEBUG
public:
    bool debug_checkMigratingThread();

protected:
    std::atomic<std::thread::id> debug_m_migratingThreadId;
    bool debug_checkSettingLegal();
#endif

#pragma mark - Migration
public:
    MigrationSetting *getMigrationSetting();

protected:
    MigrationSetting m_setting;

#pragma mark - Override
protected:
    std::shared_ptr<Handle> generateHandle() override;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* MigrationHandlePool_hpp */

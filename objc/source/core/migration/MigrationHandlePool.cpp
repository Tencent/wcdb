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

#include <WCDB/MigrationHandle.hpp>
#include <WCDB/MigrationHandlePool.hpp>

namespace WCDB {
#pragma mark - Initialize
MigrationHandlePool::MigrationHandlePool(const std::string &path,
                                         const Configs &configs)
    : HandlePool(path, configs)
{
}

#pragma mark - Migration
MigrationInfo *MigrationHandlePool::getMigrationInfo() const
{
    return m_info.get();
}

void MigrationHandlePool::setMigrationInfo(
    const std::shared_ptr<MigrationInfo> &migrationInfo)
{
    assert(migrationInfo == nullptr ||
           migrationInfo->isSameDatabaseMigration() ||
           migrationInfo->getSourceDatabasePath() != path);
    m_info = migrationInfo;
}

#pragma mark - Override
std::shared_ptr<Handle> MigrationHandlePool::generateHandle()
{
    return MigrationHandle::handleWithPath(path, tag);
}

RecyclableHandle MigrationHandlePool::flowOut()
{
    RecyclableHandle recyclableHandle = HandlePool::flowOut();
    if (recyclableHandle != nullptr) {
        static_cast<MigrationHandle *>(recyclableHandle.getHandle())
            ->setMigrationInfo(m_info);
    }
    return recyclableHandle;
}

} //namespace WCDB

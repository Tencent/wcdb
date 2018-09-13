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

#include <WCDB/Assertion.hpp>
#include <WCDB/Core.h>
#include <WCDB/Dispatch.hpp>
#include <future>

namespace WCDB {

#pragma mark - Initializer
MigrationDatabase::MigrationDatabase(const std::string &path) : Database(path)
{
}

#pragma mark - Migration
bool MigrationDatabase::stepMigration(bool &done)
{
    return false;
}

void MigrationDatabase::asyncMigration(const SteppedCallback &callback)
{
}

void MigrationDatabase::asyncMigration(double interval, int retryTimes)
{
    asyncMigration([interval, retryTimes](bool result, bool done) mutable -> bool {
        if (!done) {
            if (!result && --retryTimes == 0) {
                return false;
            }
            std::this_thread::sleep_for(
            std::chrono::microseconds((long long) (interval * 1000000)));
        }
        return true;
    });
}

bool MigrationDatabase::interruptIfDeposited()
{
    if (canRetrieve()) {
        Error error;
        error.setCode(Error::Code::Interrupt);
        error.message = "Migration stepping should be run after retrieved.";
        error.infos.set("Path", getPath());
        Notifier::shared()->notify(error);
        setThreadedError(std::move(error));
        return true;
    }
    return false;
}

#pragma mark -Handle
std::shared_ptr<Handle> MigrationDatabase::generateHandle()
{
    return std::shared_ptr<Handle>(new MigrationHandle(path, &m_infos));
}

} //namespace WCDB

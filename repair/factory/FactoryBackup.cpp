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

#include <WCDB/Data.hpp>
#include <WCDB/Factory.hpp>
#include <WCDB/FactoryBackup.hpp>

namespace WCDB {

namespace Repair {

FactoryBackup::~FactoryBackup() = default;

bool FactoryBackup::work(const UnsafeStringView &database)
{
    Backup backup(database);
    backup.setBackupSharedDelegate(m_sharedDelegate);
    backup.setBackupExclusiveDelegate(m_exclusiveDelegate);
    backup.filter(factory.getFilter());
    if (!backup.work()) {
        // Treat database empty error as succeed
        if (backup.getError().code() == Error::Code::Empty) {
            return true;
        }
        setError(backup.getError());
        return false;
    }

    auto materialPath = Factory::materialForSerializingForDatabase(database);
    if (materialPath.has_value() && backup.getMaterial().serialize(materialPath.value())) {
        return true;
    }

    assignWithSharedThreadedError();
    return false;
}

} //namespace Repair

} //namespace WCDB

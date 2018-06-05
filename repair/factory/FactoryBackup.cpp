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

#include <WCDB/Compression.hpp>
#include <WCDB/Data.hpp>
#include <WCDB/Factory.hpp>
#include <WCDB/FactoryBackup.hpp>

namespace WCDB {

namespace Repair {

FactoryBackup::FactoryBackup(const Factory &factory) : FactoryRelated(factory)
{
}

bool FactoryBackup::work(const Filter &shouldTableDeconstructed)
{
    return doWork(shouldTableDeconstructed, factory.database);
}

bool FactoryBackup::doWork(const Filter &shouldTableDeconstructed,
                           const std::string &path)
{
    Backup Backup(path);
    Backup.filter(shouldTableDeconstructed);
    if (!Backup.work()) {
        return false;
    }

    bool succeed;
    std::string materialPath;
    std::tie(succeed, materialPath) = Factory::pickMaterialForOverwriting(path);
    if (!succeed) {
        return false;
    }

    return Backup.getMaterial().serialize(materialPath);
}

} //namespace Repair

} //namespace WCDB

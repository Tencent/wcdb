//
// Created by sanhuazhang on 2018/06/05
//

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

#include "FactoryDepositor.hpp"
#include "Factory.hpp"
#include "FileManager.hpp"
#include "Path.hpp"

namespace WCDB {

namespace Repair {

FactoryDepositor::~FactoryDepositor() = default;

bool FactoryDepositor::work()
{
    auto optionalWorkshopDirectory = factory.getUniqueWorkshopDiectory();
    if (!optionalWorkshopDirectory.succeed()) {
        assignWithSharedThreadedError();
        return false;
    }

    StringView& workshopDirectory = optionalWorkshopDirectory.value();
    if (!FileManager::createDirectoryWithIntermediateDirectories(workshopDirectory)
        || !FileManager::moveItems(
        Factory::associatedPathsForDatabase(factory.database), workshopDirectory)) {
        assignWithSharedThreadedError();
        return false;
    }
    return true;
}

} //namespace Repair

} //namespace WCDB

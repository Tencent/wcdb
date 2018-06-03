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
#include <WCDB/FactoryDeconstructor.hpp>

namespace WCDB {

namespace Repair {

FactoryDeconstructor::FactoryDeconstructor(const Factory &factory)
    : FactoryDerived(factory)
{
}

bool FactoryDeconstructor::work(const Filter &shouldTableDeconstructed)
{
    Deconstructor deconstructor(factory.database);
    deconstructor.filter(shouldTableDeconstructed);
    if (!deconstructor.work()) {
        return false;
    }
    Data material = deconstructor.getMaterial().serialize();
    if (material.empty()) {
        return false;
    }

    bool succeed;
    std::string materialPath;
    std::tie(succeed, materialPath) =
        Factory::pickMaterialForOverwriting(factory.database);
    if (!succeed) {
        return false;
    }

    //TODO cipher
    FileHandle fileHandle(materialPath);
    if (!fileHandle.open(FileHandle::Mode::ReadWrite) ||
        !fileHandle.write(material.buffer(), 0, material.size())) {
        succeed = false;
    }
    fileHandle.close();
    return succeed;
}

} //namespace Repair

} //namespace WCDB

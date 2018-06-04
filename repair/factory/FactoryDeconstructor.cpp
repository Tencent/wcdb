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
    return doWork(shouldTableDeconstructed, factory.database);
}

bool FactoryDeconstructor::doWork(const Filter &shouldTableDeconstructed,
                                  const std::string &path)
{
    Deconstructor deconstructor(path);
    deconstructor.filter(shouldTableDeconstructed);
    if (!deconstructor.work()) {
        return false;
    }

    bool succeed;
    std::string materialPath;
    std::tie(succeed, materialPath) = Factory::pickMaterialForOverwriting(path);
    if (!succeed) {
        return false;
    }

    return deconstructor.getMaterial().serialize(materialPath);
}

} //namespace Repair

} //namespace WCDB

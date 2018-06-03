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

#include <WCDB/Factory.hpp>
#include <WCDB/FactoryMaterials.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

FactoryMaterials::FactoryMaterials(const Factory &factory)
    : FactoryDerived(factory), m_done(std::async([this]() -> bool {
        //TODO re-do when failed, skip corruption
        return doWork();
    }))
{
}

bool FactoryMaterials::work()
{
    return m_done.get();
}

bool FactoryMaterials::doWork()
{
    const std::string databaseName = Path::getFileName(factory.database);
    bool succeed;
    std::list<std::string> materialDirectories;
    std::tie(succeed, materialDirectories) = factory.getMaterialDirectories();
    if (!succeed) {
        return false;
    }
    for (const auto &materialDirectory : materialDirectories) {
        std::string database =
            Path::addComponent(materialDirectory, databaseName);
        std::string material;
        std::tie(succeed, material) =
            Factory::pickMaterialForRestoring(database);
        if (!succeed) {
            break;
        }
        if (material.empty()) {
            continue;
        }
        std::map<std::string, int64_t> sequence;
        std::tie(succeed, sequence) = Material::acquireMetas(material);
        if (!succeed) {
            break;
        }
        for (const auto &element : sequence) {
            auto iter = m_sequences.find(element.first);
            if (iter != m_sequences.end()) {
                iter->second = std::max(iter->second, element.second);
            } else {
                m_sequences[element.first] = element.second;
            }
        }
    }
    if (succeed) {
        return true;
    }
    m_error = std::move(ThreadedErrors::shared()->getThreadedError());
    return false;
}

const std::map<std::string, int64_t> &FactoryMaterials::getSequences() const
{
    return m_sequences;
}

} //namespace Repair

} //namespace WCDB

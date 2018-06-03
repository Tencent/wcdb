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

#ifndef FactoryMaterials_hpp
#define FactoryMaterials_hpp

#include <WCDB/FactoryDerived.hpp>
#include <future>

namespace WCDB {

namespace Repair {

class FactoryMaterials : public FactoryDerived {
public:
    FactoryMaterials(const Factory &factory);
    bool work();

    const std::map<std::string, int64_t> &getSequences() const;

protected:
    bool doWork();
    std::future<bool> m_done;
    std::map<std::string, int64_t> m_sequences;
    Error m_error;
};

} //namespace Repair

} //namespace WCDB

#endif /* FactoryMaterials_hpp */

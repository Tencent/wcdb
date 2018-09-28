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

#ifndef MigrationInitializerHandle_hpp
#define MigrationInitializerHandle_hpp

#include <WCDB/Abstract.h>
#include <WCDB/Migration.hpp>

namespace WCDB {

class MigrationInitializerHandle : public Handle, public Migration::Initializer {
public:
    using Handle::Handle;

    const Error& getError() const override;

protected:
    std::pair<bool, std::set<std::string>> getAllExistingTables() override;
    std::pair<bool, std::set<std::string>>
    getAllColumns(const std::string& table, const std::string& database) override;

    void setError(const Error& error) override;

protected:
    bool lazyOpen();
};

} // namespace WCDB

#endif /* MigrationInitializerHandle_hpp */

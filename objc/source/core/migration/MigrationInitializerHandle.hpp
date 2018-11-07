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

#ifndef _WCDB_MIGRATIONINITIALIZERHANDLE_HPP
#define _WCDB_MIGRATIONINITIALIZERHANDLE_HPP

#include <WCDB/Abstract.h>
#include <WCDB/Migration.hpp>

namespace WCDB {

class MigrationInitializerHandle final : public Handle, public Migration::Initializer {
public:
    using Handle::Handle;

    const Error& getError() const override final;

protected:
    std::pair<bool, std::set<String>> getAllExistingTables() override final;
    std::pair<bool, std::set<String>>
    getAllColumns(const String& table, const String& database) override final;

    void setError(const Error& error) override final;

protected:
    bool lazyOpen();
};

} // namespace WCDB

#endif /* _WCDB_MIGRATIONINITIALIZERHANDLE_HPP */

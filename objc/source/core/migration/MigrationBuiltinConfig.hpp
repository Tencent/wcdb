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

#ifndef MigrationBuiltinConfig_hpp
#define MigrationBuiltinConfig_hpp

#include <WCDB/BuiltinConfig.hpp>
#include <WCDB/Config.hpp>
#include <WCDB/MigrationHandle.hpp>
#include <WCDB/MigrationSetting.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class MigrationBuiltinConfig {
public:
    enum Order : int {
        Migration = INT_MIN + 4,
    };

    // 1. Attach necessary schemas and detach unnecessary schemas
    // 2. Create necessary views and drop unnecessary views
    static const Config migrationWithSetting(MigrationSetting *setting);
    static const std::string &migrationConfigName();

protected:
    static bool doCreateView(Handle *handle,
                             MigrationSetting *setting,
                             bool &schemaChanged);
    static bool doAttachSchema(Handle *handle, MigrationSetting *setting);
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* MigrationBuiltinConfig_hpp */

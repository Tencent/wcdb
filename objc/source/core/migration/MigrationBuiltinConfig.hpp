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
#include <WCDB/MigrationInfo.hpp>

namespace WCDB {

class MigrationBuiltinConfig {
public:
    enum Order : int {
        Migration = -1,
    };

    static const Config
    autoAttachWithInfo(const std::shared_ptr<MigrationInfo> &info);

    static const Config autoDetach();

    static Configs
    defaultConfigsWithMigrationInfo(const std::shared_ptr<MigrationInfo> &info);

protected:
    static bool autoDetachCallback(Handle *handle);
};

} //namespace WCDB

#endif /* MigrationBuiltinConfig_hpp */

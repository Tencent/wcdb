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

#ifndef BackupConfig_hpp
#define BackupConfig_hpp

#include <WCDB/Config.hpp>

namespace WCDB {

class BackupConfig : public Config {
public:
    static const std::shared_ptr<Config> &shared();

    BackupConfig();
    bool invoke(Handle *handle) override;

    static constexpr const char *name = "WCDBBackup";
    static constexpr const int framesForMandatoryCheckpoint = 10000;
};

} //namespace WCDB

#endif /* BackupConfig_hpp */

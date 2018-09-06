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

#ifndef MigrationConfig_hpp
#define MigrationConfig_hpp

#include <WCDB/Config.hpp>
#include <WCDB/MigrationSetting.hpp>

namespace WCDB {

class MigrationConfig : public Config {
public:
    MigrationConfig(MigrationSetting *setting);
    bool invoke(Handle *handle) override;

protected:
    MigrationSetting *m_setting;

    bool doAttachSchema(Handle *handle) const;
    bool doCreateView(Handle *handle, bool &schemaChanged) const;
};

} //namespace WCDB

#endif /* MigrationConfig_hpp */

//
// Created by sanhuazhang on 2018/06/12
//

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

#pragma once

#include "Assemble.hpp"
#include "Backup.hpp"
#include "ErrorProne.hpp"
#include "FactoryRelated.hpp"

namespace WCDB {

namespace Repair {

class FactoryRenewer final : public FactoryRelated,
                             public ErrorProne,
                             public CipherDelegateHolder,
                             public AssembleDelegateHolder,
                             public BackupDelegateHolder {
public:
    FactoryRenewer(const Factory &factory);
    ~FactoryRenewer() override;

    const StringView directory;
    const StringView database;

    bool prepare();
    bool work();

protected:
    struct Info {
        Info();
        StringView sql;
        int64_t sequence;
        std::list<StringView> associatedSQLs;
    };
    typedef struct Info Info;

    bool resolveInfosForDatabase(StringViewMap<Info> &infos,
                                 const UnsafeStringView &databaseForAcquisition);

private:
    bool assembleInfos(const StringViewMap<Info> &infos);
    bool doAssembleInfos(const StringViewMap<Info> &infos);
};

} //namespace Repair

} //namespace WCDB

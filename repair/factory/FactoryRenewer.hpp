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

#include <WCDB/Assembler.hpp>
#include <WCDB/ErrorProne.hpp>
#include <WCDB/FactoryRelated.hpp>

namespace WCDB {

namespace Repair {

class FactoryRenewer final : public FactoryRelated,
                             public ErrorProne,
                             public AssemblerHolder,
                             public LockerHolder {
public:
    FactoryRenewer(const Factory &factory);
    const StringView directory;
    const StringView database;

    bool prepare();
    bool work();

protected:
    struct Info {
        Info();
        StringView sql;
        int64_t sequence;
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

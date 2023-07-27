//
// Created by sanhuazhang on 2018/06/15
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

#include "MappedData.hpp"
#include "WCDBError.hpp"

namespace WCDB {

namespace Repair {

class Wal;
class Pager;

class WalRelated {
public:
    WalRelated(Wal *wal);
    WalRelated(WalRelated &&);
    virtual ~WalRelated() = 0;
    WalRelated &operator=(WalRelated &&);

    const StringView &getWalPath() const;
    const StringView &getPagerPath() const;

protected:
    void markWalAsCorrupted(int frame, const UnsafeStringView &message);
    void setError(Error &&error);
    void assignWithSharedThreadedError();
    std::pair<uint32_t, uint32_t>
    calculateChecksum(const MappedData &data,
                      const std::pair<uint32_t, uint32_t> &checksum) const;

    Wal *m_wal;
    Pager *m_pager;
};

} //namespace Repair

} //namespace WCDB

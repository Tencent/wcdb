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

#include "WalRelated.hpp"
#include "Assertion.hpp"
#include "Pager.hpp"

namespace WCDB {

namespace Repair {

WalRelated::~WalRelated() = default;

WalRelated::WalRelated(Wal *wal) : m_wal(wal), m_pager(m_wal->m_pager)
{
    WCTAssert(m_wal != nullptr);
}

WalRelated::WalRelated(WalRelated &&other) : m_wal(other.m_wal)
{
}

WalRelated &WalRelated::operator=(WalRelated &&other)
{
    m_wal = other.m_wal;
    return *this;
}

const StringView &WalRelated::getWalPath() const
{
    return m_wal->getPath();
}

const StringView &WalRelated::getPagerPath() const
{
    return m_pager->getPath();
}

void WalRelated::setError(Error &&error)
{
    m_wal->setError(std::move(error));
}

void WalRelated::assignWithSharedThreadedError()
{
    m_wal->assignWithSharedThreadedError();
}

void WalRelated::markWalAsCorrupted(int frame, const UnsafeStringView &message)
{
    m_wal->markAsCorrupted(frame, message);
}

std::pair<uint32_t, uint32_t>
WalRelated::calculateChecksum(const MappedData &data,
                              const std::pair<uint32_t, uint32_t> &checksum) const
{
    return m_wal->calculateChecksum(data, checksum);
}

} //namespace Repair

} //namespace WCDB

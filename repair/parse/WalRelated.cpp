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

#include <WCDB/Assertion.hpp>
#include <WCDB/Pager.hpp>
#include <WCDB/WalRelated.hpp>

namespace WCDB {

namespace Repair {

WalRelated::WalRelated(Wal *wal) : m_wal(wal)
{
    WCTInnerAssert(m_wal != nullptr);
}

WalRelated::WalRelated(WalRelated &&other) : m_wal(other.m_wal)
{
}

WalRelated &WalRelated::operator=(WalRelated &&other)
{
    m_wal = other.m_wal;
    return *this;
}

void WalRelated::setError(Error &&error)
{
    m_wal->setError(std::move(error));
}

void WalRelated::assignWithSharedThreadedError()
{
    m_wal->assignWithSharedThreadedError();
}

void WalRelated::markWalAsCorrupted(int frame, const std::string &element)
{
    m_wal->markAsCorrupted(frame, element);
}

} //namespace Repair

} //namespace WCDB

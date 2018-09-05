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
#include <WCDB/Frame.hpp>
#include <WCDB/Serialization.hpp>
#include <WCDB/String.hpp>
#include <WCDB/Wal.hpp>

namespace WCDB {

namespace Repair {

Frame::Frame(int frameno_, Wal *wal) : WalRelated(wal), frameno(frameno_)
{
    WCTInnerAssert(m_wal != nullptr);
}

int Frame::getPageNumber() const
{
    WCTInnerAssert(isInitialized());
    return m_pageno;
}

const std::pair<uint32_t, uint32_t> &Frame::getChecksum() const
{
    WCTInnerAssert(isInitialized());
    return m_checksum;
}

std::pair<bool, Page::Type> Frame::getPageType() const
{
    WCTInnerAssert(isInitialized());
    MappedData pageData = m_data.subdata(headerSize, m_wal->getPageSize());
    Page page = Page(getPageNumber(), m_pager, pageData);
    return page.acquireType();
}

uint32_t Frame::getTruncate() const
{
    WCTInnerAssert(isInitialized());
    return m_truncate;
}

std::pair<uint32_t, uint32_t>
Frame::calculateChecksum(const std::pair<uint32_t, uint32_t> &source) const
{
    WCTInnerAssert(isInitialized());
    std::pair<uint32_t, uint32_t> checksum = source;
    checksum = WalRelated::calculateChecksum(m_data.subdata(0, 8), checksum);
    checksum = WalRelated::calculateChecksum(
    m_data.subdata(headerSize, m_wal->getPageSize()), checksum);
    return checksum;
}

#pragma mark - Initializeable
bool Frame::doInitialize()
{
    if (m_data.empty()) {
        m_data = m_wal->acquireFrameData(frameno);
        if (m_data.empty()) {
            return false;
        }
    }
    Deserialization deserialization(m_data);

    WCTInnerAssert(deserialization.canAdvance(4));
    m_pageno = (int) deserialization.advance4BytesUInt();

    WCTInnerAssert(deserialization.canAdvance(4));
    m_truncate = deserialization.advance4BytesUInt();

    std::pair<uint32_t, uint32_t> salt;
    WCTInnerAssert(deserialization.canAdvance(4));
    salt.first = deserialization.advance4BytesUInt();
    WCTInnerAssert(deserialization.canAdvance(4));
    salt.second = deserialization.advance4BytesUInt();
    if (salt != m_wal->getSalt()) {
        return false;
    }

    m_checksum.first = deserialization.advance4BytesUInt();
    m_checksum.second = deserialization.advance4BytesUInt();
    return true;
}

} //namespace Repair

} //namespace WCDB
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
#include <WCDB/Wal.hpp>

namespace WCDB {

namespace Repair {

Frame::Frame(int frameno_,
             Wal *wal,
             const std::pair<uint32_t, uint32_t> &checksum)
    : WalRelated(wal), frameno(frameno_), m_checksum(checksum)
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

std::pair<uint32_t, uint32_t>
Frame::calculateChecksum(const Data &data,
                         const std::pair<uint32_t, uint32_t> &checksum)
{
    WCTInnerAssert(data.size() >= 8);
    WCTInnerAssert((data.size() & 0x00000007) == 0);

    const uint32_t *iter = reinterpret_cast<const uint32_t *>(data.buffer());
    const uint32_t *end =
        reinterpret_cast<const uint32_t *>(data.buffer() + data.size());

    std::pair<uint32_t, uint32_t> result = checksum;

    if (m_wal->isNativeChecksum()) {
        do {
            result.first += *iter++ + result.second;
            result.second += *iter++ + result.first;
        } while (iter < end);
    } else {
        do {
#define BYTESWAP32(x)                                                          \
    ((((x) &0x000000FF) << 24) + (((x) &0x0000FF00) << 8) +                    \
     (((x) &0x00FF0000) >> 8) + (((x) &0xFF000000) >> 24))
            result.first += BYTESWAP32(iter[0]) + result.second;
            result.second += BYTESWAP32(iter[1]) + result.first;
            iter += 2;
        } while (iter < end);
    }

    return result;
}

#pragma mark - Initializeable
bool Frame::doInitialize()
{
    Data data = m_wal->acquireFrameData(frameno);
    if (data.empty()) {
        return false;
    }
    Deserialization deserialization(data);

    WCTInnerAssert(deserialization.canAdvance(4));
    m_pageno = (int) deserialization.advance4BytesUInt();

    WCTInnerAssert(deserialization.canAdvance(4));
    deserialization.advance(4);

    std::pair<uint32_t, uint32_t> salt;
    WCTInnerAssert(deserialization.canAdvance(4));
    salt.first = deserialization.advance4BytesUInt();
    WCTInnerAssert(deserialization.canAdvance(4));
    salt.second = deserialization.advance4BytesUInt();
    if (salt != m_wal->getSalt()) {
        markWalAsCorrupted();
        return false;
    }

    std::pair<uint32_t, uint32_t> checksum;
    checksum.first = deserialization.advance4BytesUInt();
    checksum.second = deserialization.advance4BytesUInt();

    m_checksum = calculateChecksum(data.subdata(0, 8), m_checksum);
    m_checksum = calculateChecksum(
        data.subdata(headerSize, m_wal->getPageSize()), m_checksum);
    if (m_checksum == checksum) {
        return true;
    }
    markWalAsCorrupted();
    return false;
}

} //namespace Repair

} //namespace WCDB

//
// Created by sanhuazhang on 2018/06/08
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

#include "Wal.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "FileManager.hpp"
#include "Frame.hpp"
#include "Notifier.hpp"
#include "Pager.hpp"
#include "Path.hpp"
#include "SQLite.h"
#include "Serialization.hpp"
#include "StringView.hpp"

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Wal::Wal(Pager *pager)
: PagerRelated(pager)
, m_fileHandle(Path::addExtention(m_pager->getPath(), "-wal"))
, m_fileSize(0)
, m_truncate(std::numeric_limits<uint32_t>::max())
, m_maxFrame(0)
, m_isNativeChecksum(false)
, m_salt({ 0, 0 })
, m_shmLegality(true)
, m_shm(this)
{
}

Wal::~Wal() = default;

const StringView &Wal::getPath() const
{
    return m_fileHandle.path;
}

MappedData Wal::acquireData(offset_t offset, size_t size, SharedHighWater highWater)
{
    WCTAssert(m_fileHandle.isOpened());
    MappedData data = m_fileHandle.map(offset, size, highWater);
    if (data.size() != size) {
        if (data.size() > 0) {
            markAsCorrupted((int) ((offset - headerSize) / getFrameSize() + 1),
                            StringView::formatted("Acquired wal data with size: %d is less than the expected size: %d.",
                                                  data.size(),
                                                  size));
        } else {
            assignWithSharedThreadedError();
        }
        return MappedData::null();
    }
    return data;
}

#pragma mark - Page
bool Wal::containsPage(int pageno) const
{
    return m_pages2Frames.find(pageno) != m_pages2Frames.end();
}

MappedData Wal::acquirePageData(int pageno, SharedHighWater highWater)
{
    return acquirePageData(pageno, 0, getPageSize(), highWater);
}

MappedData Wal::acquirePageData(int pageno, offset_t offset, size_t size, SharedHighWater highWater)
{
    WCTAssert(isInitialized());
    WCTAssert(containsPage(pageno));
    WCTAssert(offset + size <= getPageSize());
    return acquireData(headerSize + getFrameSize() * (m_pages2Frames[pageno] - 1)
                       + Frame::headerSize + offset,
                       size,
                       highWater);
}

int Wal::getMaxPageno() const
{
    if (m_pages2Frames.empty()) {
        return 0;
    }
    return m_pages2Frames.rbegin()->first;
}

#pragma mark - Wal
bool Wal::isCheckpointIncreasedSalt(const Salt &before, const Salt &after)
{
    return before.first + 1 == after.first && before.second != after.second;
}

void Wal::setShmLegality(bool flag)
{
    WCTAssert(!isInitialized());
    m_shmLegality = flag;
}

MappedData Wal::acquireFrameData(int frameno)
{
    WCTAssert(isInitializing());
    return acquireData(headerSize + getFrameSize() * (frameno - 1), getFrameSize());
}

int Wal::getNBackFill() const
{
    WCTAssert(isInitialized());
    return m_shm.getBackfill();
}

void Wal::setNBackFill(int nbackfill)
{
    WCTAssert(!isInitialized());
    m_nbackfill = nbackfill;
}

int Wal::getMaxFrame() const
{
    return m_maxFrame;
}

int Wal::getNumberOfFrames() const
{
    return (int) m_pages2Frames.size();
}

int Wal::getPageSize() const
{
    return m_pager->getPageSize();
}

int Wal::getFrameSize() const
{
    return Frame::headerSize + getPageSize();
}

const std::pair<uint32_t, uint32_t> &Wal::getSalt() const
{
    return m_salt;
}

void Wal::setSalt(const std::pair<uint32_t, uint32_t> &salt)
{
    WCTAssert(!isInitialized());
    m_salt = salt;
}

bool Wal::isBigEndian()
{
    static bool s_isBigEndian = []() -> bool {
        short int n = 0x1;
        char *p = (char *) &n;
        return p[0] != 1;
    }();
    return s_isBigEndian;
}

std::pair<uint32_t, uint32_t>
Wal::calculateChecksum(const MappedData &data, const std::pair<uint32_t, uint32_t> &checksum) const
{
    WCTAssert(data.size() >= 8);
    WCTAssert((data.size() & 0x00000007) == 0);

    const uint32_t *iter = reinterpret_cast<const uint32_t *>(data.buffer());
    const uint32_t *end
    = reinterpret_cast<const uint32_t *>(data.buffer() + data.size());

    std::pair<uint32_t, uint32_t> result = checksum;

    if (m_isNativeChecksum) {
        do {
            result.first += *iter++ + result.second;
            result.second += *iter++ + result.first;
        } while (iter < end);
    } else {
        do {
#define BYTESWAP32(x)                                                          \
    ((((x) &0x000000FF) << 24) + (((x) &0x0000FF00) << 8)                      \
     + (((x) &0x00FF0000) >> 8) + (((x) &0xFF000000) >> 24))
            result.first += BYTESWAP32(iter[0]) + result.second;
            result.second += BYTESWAP32(iter[1]) + result.first;
            iter += 2;
        } while (iter < end);
    }

    return result;
}

bool Wal::doInitialize()
{
    WCTAssert(m_pager->isInitialized() || m_pager->isInitializing());

    int maxWalFrame = std::numeric_limits<int>::max();
    if (m_shmLegality) {
        if (!m_shm.initialize()) {
            return false;
        }
        WCTAssert(m_nbackfill == 0 || (m_salt == Salt{ 0, 0 })
                  || (m_nbackfill == m_shm.getBackfill() && m_salt == m_shm.getSalt())
                  || (m_shm.getBackfill() == 0
                      && isCheckpointIncreasedSalt(m_salt, m_shm.getSalt())));
        m_nbackfill = m_shm.getBackfill();
        m_salt = m_shm.getSalt();
        maxWalFrame = m_shm.getMaxFrame();
    }

    auto fileSize = FileManager::getFileSize(getPath());
    if (!fileSize.succeed()) {
        assignWithSharedThreadedError();
        return false;
    }
    m_fileSize = fileSize.value();
    if (m_fileSize == 0) {
        return true;
    }

    if (!m_fileHandle.open(FileHandle::Mode::ReadOnly)) {
        assignWithSharedThreadedError();
        return false;
    }
    FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(getPath());

    MappedData data = acquireData(0, headerSize);
    if (data.empty()) {
        return false;
    }
    Deserialization deserialization(data);
    uint32_t magic = deserialization.advance4BytesUInt();
    if ((magic & 0xFFFFFFFE) != 0x377F0682) {
        // ignore wal
        markAsCorrupted(0, StringView::formatted("Incorrect wal magic: 0x%x.", magic));
        return false;
    }
    m_isNativeChecksum = (magic & 0x00000001) == isBigEndian();
    deserialization.seek(16);
    std::pair<uint32_t, uint32_t> salt = { 0, 0 };
    WCTAssert(deserialization.canAdvance(4));
    salt.first = deserialization.advance4BytesUInt();
    WCTAssert(deserialization.canAdvance(4));
    salt.second = deserialization.advance4BytesUInt();

    if (m_salt != Salt{ 0, 0 } && m_salt != salt
        && !(isCheckpointIncreasedSalt(m_salt, salt) && m_nbackfill > 0)) {
        m_nbackfill = 0;
        maxWalFrame = std::numeric_limits<int>::max();
    }
    m_salt = salt;

    const int numberOfFramesInFile = ((int) m_fileSize - headerSize) / getFrameSize();
    maxWalFrame = std::min(numberOfFramesInFile, maxWalFrame);

    if (m_nbackfill >= maxWalFrame) {
        // dispose all wal frames since they are already checkpointed.
        return true;
    }

    std::pair<uint32_t, uint32_t> checksum = { 0, 0 };
    checksum = calculateChecksum(data.subdata(headerSize - 2 * sizeof(uint32_t)), checksum);

    std::pair<uint32_t, uint32_t> deserializedChecksum;
    WCTAssert(deserialization.canAdvance(4));
    deserializedChecksum.first = deserialization.advance4BytesUInt();
    WCTAssert(deserialization.canAdvance(4));
    deserializedChecksum.second = deserialization.advance4BytesUInt();

    if (checksum != deserializedChecksum) {
        markAsCorrupted(0,
                        StringView::formatted("Mismatched wal checksum: %u, %u to %u, %u.",
                                              checksum.first,
                                              checksum.second,
                                              deserializedChecksum.first,
                                              deserializedChecksum.second));
        return false;
    }

    if (m_nbackfill > 0) {
        Frame frame(m_nbackfill, this);
        if (!frame.initialize()) {
            dispose();
            return false;
        }
        checksum = frame.getChecksum();
    }

    std::map<int, int> committedRecords;
    for (int frameno = m_nbackfill + 1; frameno <= maxWalFrame; ++frameno) {
        Frame frame(frameno, this);
        if (!frame.initialize()) {
            dispose();
            return false;
        }
        checksum = frame.calculateChecksum(checksum);
        if (checksum != frame.getChecksum()) {
            if (m_shmLegality) {
                //If the frame checksum is mismatched and shm is legal, it mean to be corrupted.
                markAsCorrupted(frameno,
                                StringView::formatted("Mismatched frame checksum: %u, %u to %u, %u.",
                                                      frame.getChecksum().first,
                                                      frame.getChecksum().second,
                                                      checksum.first,
                                                      checksum.second));
                return false;
            } else {
                //If the frame checksum is mismatched and shm is not legal, it mean to be disposed.
                break;
            }
        }
        checksum = frame.getChecksum();
        committedRecords[frame.getPageNumber()] = frameno;
        if (frame.getTruncate() != 0) {
            m_truncate = frame.getTruncate();
            m_maxFrame = frameno;
            for (const auto &element : committedRecords) {
                m_pages2Frames[element.first] = element.second;
            }
            committedRecords.clear();
        }
    }
    if (!committedRecords.empty()) {
        for (const auto &iter : committedRecords) {
            m_disposedPages.emplace(iter.first);
        }
    }
    // all those frames that are uncommitted or exceeds the max allowed count will be disposed.
    return true;
}

#pragma mark - Error
void Wal::markAsCorrupted(int frame, const UnsafeStringView &message)
{
    Error error(Error::Code::Corrupt, Error::Level::Notice, message);
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, getPagerPath());
    error.infos.insert_or_assign("Frame", frame);
    Notifier::shared().notify(error);
    setError(std::move(error));
}

#pragma mark - Dispose
int Wal::getDisposedPages() const
{
    return (int) m_disposedPages.size();
}

void Wal::dispose()
{
    for (const auto &element : m_pages2Frames) {
        m_disposedPages.emplace(element.first);
    }
    m_pages2Frames.clear();
    m_truncate = std::numeric_limits<uint32_t>::max();
    m_fileSize = 0;
    m_maxFrame = 0;
    m_salt = { 0, 0 };
}

} //namespace Repair

} //namespace WCDB

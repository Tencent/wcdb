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
#include <WCDB/FileManager.hpp>
#include <WCDB/Frame.hpp>
#include <WCDB/Pager.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/Serialization.hpp>
#include <WCDB/Wal.hpp>

namespace WCDB {

namespace Repair {

Wal::Wal(Pager *pager)
    : PagerRelated(pager)
    , m_fileHandle(Path::addExtention(m_pager->getPath(), "-wal"))
    , m_salt({0, 0})
    , m_checksum({0, 0})
    , m_isNativeChecksum(false)
    , m_maxFrame(std::numeric_limits<int>::max())
    , m_frames(0)
{
}

void Wal::setMaxFrame(int maxFrame)
{
    WCTInnerAssert(!isInitialized());
    m_maxFrame = maxFrame;
}

int Wal::getFrameCount() const
{
    WCTInnerAssert(isInitialized());
    return m_frames;
}

const std::string &Wal::getPath() const
{
    return m_fileHandle.path;
}

bool Wal::containsPage(int pageno) const
{
    WCTInnerAssert(isInitialized());
    return m_framePages.find(pageno) != m_framePages.end();
}

Data Wal::acquirePageData(int pageno)
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(containsPage(pageno));
    return acquireData(headerSize + getFrameSize() * m_framePages[pageno] +
                           Frame::headerSize,
                       getPageSize());
}

Data Wal::acquireFrameData(int frameno)
{
    WCTInnerAssert(isInitializing());
    return acquireData(headerSize + getFrameSize() * (frameno - 1),
                       getFrameSize());
}

Data Wal::acquireData(off_t offset, size_t size)
{
    if (!m_fileHandle.isOpened() &&
        !m_fileHandle.open(FileHandle::Mode::ReadOnly)) {
        assignWithSharedThreadedError();
        return Data::emptyData();
    }
    Data data(size);
    if (data.empty()) {
        assignWithSharedThreadedError();
        return data;
    }
    ssize_t read = m_fileHandle.read(data.buffer(), offset, size);
    if (read != size) {
        if (read >= 0) {
            //short read
            markAsCorrupted();
        } else {
            assignWithSharedThreadedError();
        }
        return Data::emptyData();
    }
    return data;
}

int Wal::getPageSize() const
{
    return m_pager->getPageSize();
}

int Wal::getFrameSize() const
{
    return Frame::headerSize + getPageSize();
}

bool Wal::isNativeChecksum() const
{
    WCTInnerAssert(isInitializing() || isInitialized());
    return m_isNativeChecksum;
}

const std::pair<uint32_t, uint32_t> &Wal::getChecksum() const
{
    WCTInnerAssert(isInitializing());
    return m_checksum;
}

const std::pair<uint32_t, uint32_t> &Wal::getSalt() const
{
    WCTInnerAssert(isInitializing() || isInitialized());
    return m_salt;
}

#pragma mark - Initializeable
#if (defined(i386) || defined(__i386__) || defined(_M_IX86) ||                 \
     defined(__x86_64) || defined(__x86_64__) || defined(_M_X64) ||            \
     defined(_M_AMD64) || defined(_M_ARM) || defined(__x86) ||                 \
     defined(__arm__)) &&                                                      \
    !defined(SQLITE_RUNTIME_BYTEORDER)
#define SQLITE_BIGENDIAN 0
#endif
#if (defined(sparc) || defined(__ppc__)) && !defined(SQLITE_RUNTIME_BYTEORDER)
#define SQLITE_BIGENDIAN 1
#endif

bool Wal::doInitialize()
{
    WCTInnerAssert(m_pager->isInitialized());

    bool succeed;
    size_t fileSize;
    std::tie(succeed, fileSize) = FileManager::shared()->getFileSize(getPath());
    if (fileSize == 0) {
        return succeed;
    }

    Data data = acquireData(0, headerSize);
    if (data.empty()) {
        return false;
    }
    Deserialization deserialization(data);
    uint32_t magic = deserialization.advance4BytesUInt();
    if (magic != 0x377f0682 && magic != 0x377f0683) {
        markAsCorrupted();
        return false;
    }
    m_isNativeChecksum = (magic & 0x00000001) == SQLITE_BIGENDIAN;
    deserialization.seek(16);
    m_salt.first = deserialization.advance4BytesUInt();
    m_salt.second = deserialization.advance4BytesUInt();
    m_checksum.first = deserialization.advance4BytesUInt();
    m_checksum.second = deserialization.advance4BytesUInt();

    int frameno = 0;
    const int frameSize = getFrameSize();
    for (off_t offset = headerSize;
         offset + frameSize <= fileSize && frameno < m_maxFrame;
         offset += frameSize) {
        Frame frame(++frameno, this, m_pager);
        if (!frame.initialize()) {
            break;
        }
        m_frames = frameno;
        m_framePages[frame.getPageNumber()] = frameno;
        m_checksum = frame.getChecksum();
    }
    return true;
}

} //namespace Repair

} //namespace WCDB

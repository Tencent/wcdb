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
#include <WCDB/String.hpp>
#include <WCDB/Wal.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Wal::Wal(Pager *pager)
: PagerRelated(pager)
, m_fileHandle(Path::addExtention(m_pager->getPath(), "-wal"))
, m_salt({ 0, 0 })
, m_isNativeChecksum(false)
, m_maxFrame(std::numeric_limits<int>::max())
, m_frames(0)
{
}

const std::string &Wal::getPath() const
{
    return m_fileHandle.path;
}

MappedData Wal::acquireData(off_t offset, size_t size)
{
    WCTInnerAssert(m_fileHandle.isOpened());
    MappedData data = m_fileHandle.map(offset, size);
    if (data.size() != size) {
        if (data.size() > 0) {
            markAsCorrupted((int) ((offset - headerSize) / getFrameSize() + 1),
                            String::formatted("Acquired wal data with size: %d is less than the expected size: %d.",
                                              data.size(),
                                              size));
        } else {
            assignWithSharedThreadedError();
        }
        return MappedData::emptyData();
    }
    return data;
}

#pragma mark - Page
bool Wal::containsPage(int pageno) const
{
    WCTInnerAssert(isInitialized());
    return m_framePages.find(pageno) != m_framePages.end();
}

MappedData Wal::acquirePageData(int pageno)
{
    return acquirePageData(pageno, 0, getPageSize());
}

MappedData Wal::acquirePageData(int pageno, off_t offset, size_t size)
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(containsPage(pageno));
    WCTInnerAssert(offset + size <= getPageSize());
    return acquireData(headerSize + getFrameSize() * (m_framePages[pageno] - 1)
                       + Frame::headerSize + offset,
                       size);
}

int Wal::getMaxPageno() const
{
    if (m_framePages.empty()) {
        return 0;
    }
    return m_framePages.rbegin()->first;
}

#pragma mark - Wal
MappedData Wal::acquireFrameData(int frameno)
{
    WCTInnerAssert(isInitializing());
    return acquireData(headerSize + getFrameSize() * (frameno - 1), getFrameSize());
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

const std::pair<uint32_t, uint32_t> &Wal::getSalt() const
{
    WCTInnerAssert(isInitializing() || isInitialized());
    return m_salt;
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

bool Wal::doInitialize()
{
    WCTInnerAssert(m_pager->isInitialized() || m_pager->isInitializing());

    bool succeed;
    size_t fileSize;
    std::tie(succeed, fileSize) = FileManager::getFileSize(getPath());
    if (fileSize == 0) {
        if (!succeed) {
            assignWithSharedThreadedError();
        }
        return succeed;
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
    if ((magic & 0xFFFFFFFE) != 0x377f0682) {
        markAsCorrupted(0, String::formatted("Magic number: 0x%x is illegal.", magic));
        return false;
    }
    m_isNativeChecksum = (magic & 0x00000001) == isBigEndian();
    deserialization.seek(16);
    m_salt.first = deserialization.advance4BytesUInt();
    m_salt.second = deserialization.advance4BytesUInt();
    std::pair<uint32_t, uint32_t> checksum;
    checksum.first = deserialization.advance4BytesUInt();
    checksum.second = deserialization.advance4BytesUInt();

    const int frameSize = getFrameSize();
    const int framesSize = (int) fileSize - headerSize;
    const int frameCountInFile = framesSize / frameSize;
    for (int frameno = 1; frameno <= frameCountInFile; ++frameno) {
        Frame frame(frameno, this, checksum);
        if (!frame.initialize()) {
            //If frame can't be inited, it means that it' already corrupted or only a reused frame.
            //If it's the latter one, it doesn't mean disposed.
            break;
        }
        if (frameno <= m_maxFrame) {
            m_frames = frameno;
            m_framePages[frame.getPageNumber()] = frameno;
        } else {
            bool succeed;
            Page::Type pageType;
            std::tie(succeed, pageType) = frame.getPageType();
            if (!succeed || pageType == Page::Type::LeafTable
                || pageType == Page::Type::Unknown) {
                m_disposedPages.emplace(frame.getPageNumber());
                Error error;
                error.level = Error::Level::Notice;
                error.setCode(Error::Code::Notice, "Repair");
                error.message = "Dispose wal frame that is not backed up.";
                error.infos.set("Frame", frameno);
                error.infos.set("Page", frame.getPageNumber());
                error.infos.set("Path", getPath());
                Notifier::shared()->notify(error);
            }
        }
        checksum = frame.getChecksum();
    }
    return true;
}

#pragma mark - Error
void Wal::markAsCorrupted(int frame, const std::string &message)
{
    Error error;
    error.setCode(Error::Code::Corrupt, "Repair");
    error.message = message;
    error.infos.set("Path", getPath());
    error.infos.set("Frame", frame);
    Notifier::shared()->notify(error);
    setError(std::move(error));
}

void Wal::markAsError(Error::Code code)
{
    Error error;
    error.setCode(code, "Repair");
    error.infos.set("Path", getPath());
    Notifier::shared()->notify(error);
    setError(std::move(error));
}

#pragma mark - Dispose
int Wal::getDisposedPage() const
{
    WCTInnerAssert(isInitialized());
    return (int) m_disposedPages.size();
}

void Wal::dispose()
{
    WCTInnerAssert(isInitialized() || isInitializeFalied());
    for (const auto &element : m_framePages) {
        m_disposedPages.emplace(element.first);
    }
    m_framePages.clear();
    m_frames = 0;
}

} //namespace Repair

} //namespace WCDB

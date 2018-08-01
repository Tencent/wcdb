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
#include <WCDB/Pager.hpp>
#include <WCDB/Serialization.hpp>
#include <WCDB/String.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Pager::Pager(const std::string &path)
: m_fileHandle(path), m_pageSize(-1), m_reservedBytes(-1), m_pageCount(0), m_wal(this)
{
}

void Pager::setPageSize(int pageSize)
{
    WCTInnerAssert(!isInitialized());
    m_pageSize = pageSize;
}

void Pager::setReservedBytes(int reservedBytes)
{
    WCTInnerAssert(!isInitialized());
    m_reservedBytes = reservedBytes;
}

const std::string &Pager::getPath() const
{
    return m_fileHandle.path;
}

#pragma mark - Page
int Pager::getPageCount() const
{
    WCTInnerAssert(isInitialized());
    return std::max(m_wal.getMaxPageno(), m_pageCount);
}

int Pager::getUsableSize() const
{
    WCTInnerAssert(isInitialized() || isInitializing());
    return m_pageSize - m_reservedBytes;
}

int Pager::getPageSize() const
{
    WCTInnerAssert(isInitialized() || isInitializing());
    return m_pageSize;
}

int Pager::getReservedBytes() const
{
    WCTInnerAssert(isInitialized());
    return m_reservedBytes;
}

MappedData Pager::acquirePageData(int number)
{
    return acquirePageData(number, 0, m_pageSize);
}

MappedData Pager::acquirePageData(int number, off_t offset, size_t size)
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(number > 0);
    WCTInnerAssert(offset + size <= m_pageSize);
    MappedData data;
    if (m_wal.containsPage(number)) {
        data = m_wal.acquirePageData(number, offset, size);
    } else if (number > m_pageCount) {
        markAsCorrupted(
        number, String::formatted("Page number: %d exceeds the page count: %d.", number, m_pageCount));
        return MappedData::emptyData();
    } else {
        data = m_fileHandle.mapPage(number, offset, size);
    }
    if (data.size() != size) {
        if (data.size() > 0) {
            //short read
            markAsCorrupted((int) (offset / m_pageSize + 1),
                            String::formatted("Acquired page data with size: %d is less than the expected size: %d.",
                                              data.size(),
                                              size));
        } else {
            assignWithSharedThreadedError();
        }
        return MappedData::emptyData();
    }
    return data;
}

MappedData Pager::acquireData(off_t offset, size_t size)
{
    WCTInnerAssert(m_fileHandle.isOpened());
    MappedData data = m_fileHandle.map(offset, size);
    if (data.size() != size) {
        if (data.size() > 0) {
            markAsCorrupted((int) (offset / m_pageSize + 1),
                            String::formatted("Acquired data with size: %d is less than the expected size: %d.",
                                              data.size(),
                                              size));
        } else {
            assignWithSharedThreadedError();
        }
        return MappedData::emptyData();
    }
    return data;
}

#pragma mark - Wal
void Pager::setMaxWalFrame(int maxWalFrame)
{
    m_wal.setMaxFrame(maxWalFrame);
}

int Pager::getDisposedWalPage() const
{
    return m_wal.getDisposedPage();
}

void Pager::disposeWal()
{
    m_wal.dispose();
}

const std::pair<uint32_t, uint32_t> &Pager::getWalSalt() const
{
    return m_wal.getSalt();
}

int Pager::getWalFrameCount() const
{
    return m_wal.getFrameCount();
}

#pragma mark - Error
void Pager::markAsCorrupted(int page, const std::string &diagnostic)
{
    Error error;
    error.setCode(Error::Code::Corrupt, "Repair");
    error.infos.set("Path", getPath());
    error.infos.set("Diagnostic", diagnostic);
    error.infos.set("Page", page);
    Notifier::shared()->notify(error);
    setError(std::move(error));
}

void Pager::markAsError(Error::Code code)
{
    Error error;
    error.setCode(code, "Repair");
    error.infos.set("Path", getPath());
    Notifier::shared()->notify(error);
    setError(std::move(error));
}

#pragma mark - Initializeable
bool Pager::doInitialize()
{
    FileManager *fileManager = FileManager::shared();
    bool succeed;
    size_t fileSize;
    std::tie(succeed, fileSize) = fileManager->getFileSize(getPath());
    if (fileSize == 0) {
        if (succeed) {
            markAsError(Error::Code::Empty);
        } else {
            assignWithSharedThreadedError();
        }
        return false;
    }

    if (!m_fileHandle.open(FileHandle::Mode::ReadOnly)) {
        assignWithSharedThreadedError();
        return false;
    }
    fileManager->setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(getPath());
    if (m_pageSize == -1 || m_reservedBytes == -1) {
        MappedData data = acquireData(0, 100);
        if (data.empty()) {
            assignWithSharedThreadedError();
            return false;
        }
        if (memcmp(data.buffer(), "SQLite format 3\000", 16) != 0) {
            markAsError(Error::Code::NotADatabase);
            return false;
        }
        Deserialization deserialization(data);
        //parse page size
        if (m_pageSize == -1) {
            deserialization.seek(16);
            WCTInnerAssert(deserialization.canAdvance(2));
            m_pageSize = deserialization.advance2BytesInt();
        }
        //parse reserved bytes
        if (m_reservedBytes == -1) {
            deserialization.seek(20);
            WCTInnerAssert(deserialization.canAdvance(1));
            m_reservedBytes = deserialization.advance1ByteInt();
        }
    }
    if (((m_pageSize - 1) & m_pageSize) != 0 || m_pageSize < 512) {
        markAsCorrupted(
        1, String::formatted("Page size: %d is not aligned or not too small.", m_pageSize));
        return false;
    }
    if (m_reservedBytes < 0 || m_reservedBytes > 255) {
        markAsCorrupted(
        1, String::formatted("Reversed bytes: %d is illegal.", m_reservedBytes));
        return false;
    }

    m_fileHandle.setPageSize(m_pageSize);

    m_pageCount = (int) ((fileSize + m_pageSize - 1) / m_pageSize);

    if (!m_wal.initialize()) {
        if (!m_error.isCorruption()) {
            return false;
        }
        disposeWal();
    }
    return true;
}

} //namespace Repair

} //namespace WCDB

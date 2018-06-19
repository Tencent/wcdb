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
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Pager::Pager(const std::string &path)
    : m_fileHandle(path)
    , m_pageSize(-1)
    , m_reservedBytes(-1)
    , m_pageCount(0)
    , m_wal(this)
    , m_disposeWal(false)
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
    return m_disposeWal ? m_pageCount
                        : std::max(m_wal->getMaxPageno(), m_pageCount);
}

int Pager::getUsableSize() const
{
    WCTInnerAssert(isInitialized());
    return m_pageSize - m_reservedBytes;
}

int Pager::getPageSize() const
{
    WCTInnerAssert(isInitialized());
    return m_pageSize;
}

int Pager::getReservedBytes() const
{
    WCTInnerAssert(isInitialized());
    return m_reservedBytes;
}

Data Pager::acquirePageData(int number)
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(number > 0);
    if (!m_disposeWal && m_wal->containsPage(number)) {
        return m_wal->acquirePageData(number);
    }
    return acquireData((number - 1) * m_pageSize, m_pageSize);
}

Data Pager::acquireData(off_t offset, size_t size)
{
    WCTInnerAssert(isInitializing() || isInitialized());
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

#pragma mark - Wal
void Pager::setMaxWalFrame(int maxWalFrame)
{
    m_wal.setMaxFrame(maxWalFrame);
}

const std::pair<uint32_t, uint32_t> Pager::getWalSalt() const
{
    WCTInnerAssert(!m_disposeWal);
    return m_wal.getSalt();
}

int Pager::getWalFrameCount() const
{
    return m_wal.getFrameCount();
}

#pragma mark - Error
void Pager::markAsCorrupted()
{
    markAsError(Error::Code::Corrupt);
}

void Pager::markAsError(Error::Code code)
{
    Error error;
    error.setCode(code, "Repair");
    error.infos.set("Path", m_fileHandle.path);
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

    if (m_pageSize == -1 || m_reservedBytes == -1) {
        Data data = acquireData(0, 100);
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
    if (((m_pageSize - 1) & m_pageSize) != 0 || m_pageSize < 512 ||
        m_reservedBytes < 0 || m_reservedBytes > 255) {
        markAsCorrupted();
        return false;
    }

    m_pageCount = (int) ((fileSize + m_pageSize - 1) / m_pageSize);

    std::tie(succeed, fileSize) = fileManager->getFileSize(m_wal.getPath());
    if (!succeed) {
        assignWithSharedThreadedError();
        return false;
    }
    if (fileSize == 0) {
        disposeWal();
    } else {
        return m_wal.initialize();
    }
    return true;
}

#pragma mark - Dispose
bool Pager::isWalDisposed() const
{
    return m_disposeWal;
}

void Pager::disposeWal()
{
    m_disposeWal = true;
}

} //namespace Repair

} //namespace WCDB

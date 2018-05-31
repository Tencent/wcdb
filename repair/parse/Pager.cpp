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
    : m_fileHandle(path), m_pageSize(-1), m_reservedBytes(-1), m_pageCount(0)
{
}

void Pager::setPageSize(int pageSize)
{
    m_pageSize = pageSize;
}

void Pager::setReservedBytes(int reservedBytes)
{
    m_reservedBytes = reservedBytes;
}

const std::string &Pager::getPath() const
{
    return m_fileHandle.path;
}

bool Pager::initialize()
{
    if (m_pageSize == -1 || m_reservedBytes == -1) {
        Data data = acquireData(0, 100);
        if (data.empty()) {
            return false;
        }
        if (memcmp(data.buffer(), "SQLite format 3\000", 16) != 0) {
            markAsNotADatabase();
            return false;
        }
        Deserialization deserialization(data);
        //parse page size
        if (m_pageSize == -1) {
            deserialization.seek(16);
            WCTInnerAssert(deserialization.isEnough(2));
            m_pageSize = deserialization.advance2BytesInt();
        }
        //parse reserved bytes
        if (m_reservedBytes == -1) {
            deserialization.seek(20);
            WCTInnerAssert(deserialization.isEnough(1));
            m_reservedBytes = deserialization.advance1ByteInt();
        }
    }
    if (((m_pageSize - 1) & m_pageSize) != 0 || m_pageSize < 512 ||
        m_reservedBytes < 0 || m_reservedBytes > 255) {
        markAsCorrupted();
        return false;
    }
    auto fileSizePair = FileManager::shared()->getFileSize(getPath());
    if (!fileSizePair.first) {
        return false;
    }
    m_pageCount = (int) ((fileSizePair.second + m_pageSize - 1) / m_pageSize);
    return true;
}

#pragma mark - Page
int Pager::getPageCount() const
{
    return m_pageCount;
}

int Pager::getUsableSize() const
{
    return m_pageSize - m_reservedBytes;
}

int Pager::getPageSize() const
{
    return m_pageSize;
}

int Pager::getReservedBytes() const
{
    return m_reservedBytes;
}

Data Pager::acquirePageData(int number)
{
    WCTInnerAssert(number > 0);
    return acquireData((number - 1) * m_pageSize, m_pageSize);
}

Data Pager::acquireData(off_t offset, size_t size)
{
    if (!m_fileHandle.open(FileHandle::Mode::ReadOnly)) {
        return Data::emptyData();
    }
    Data data(size);
    if (data.empty()) {
        return data;
    }
    ssize_t read = m_fileHandle.read(data.buffer(), offset, size);
    if (read == size) {
        return data;
    } else if (read < 0) {
        //file handle error
    } else {
        //short read
        markAsCorrupted();
    }
    return Data::emptyData();
}

#pragma mark - Error
void Pager::markAsCorrupted()
{
    markAsError(Error::Code::Corrupt);
}

void Pager::markAsNotADatabase()
{
    markAsError(Error::Code::NotADatabase);
}

void Pager::markAsError(Error::Code code)
{
    Error error;
    error.setCode(code, "Repair");
    error.infos.set("Path", m_fileHandle.path);
    Reporter::shared()->report(error);
    setThreadedError(std::move(error));
}

} //namespace Repair

} //namespace WCDB

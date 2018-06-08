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

Wal::Wal(Pager &pager)
    : m_pager(pager)
    , m_fileHandle(Path::addExtention(pager.getPath(), "-wal"))
    , m_salt({0, 0})
    , m_checksum({0, 0})
    , m_isBigEndian(false)
{
}

const std::string &Wal::getPath() const
{
    return m_fileHandle.path;
}

bool Wal::initialize()
{
    bool succeed;
    size_t fileSize;
    std::tie(succeed, fileSize) = FileManager::shared()->getFileSize(getPath());
    if (fileSize == 0) {
        return succeed;
    }

    Data data = acquireData(0, 32);
    if (data.empty()) {
        return false;
    }
    Deserialization deserialization(data);
    uint32_t magic = deserialization.advance4BytesUInt();
    if (magic != 0x377f0682 && magic != 0x377f0683) {
        return false;
    }
    m_isBigEndian = magic & 0x00000001;
    deserialization.seek(16);
    m_salt.first = deserialization.advance4BytesUInt();
    m_salt.second = deserialization.advance4BytesUInt();
    m_checksum.first = deserialization.advance4BytesUInt();
    m_checksum.second = deserialization.advance4BytesUInt();

    int frameno = 0;
    const int frameSize = getFrameSize();
    for (off_t offset = 32; offset + frameSize <= fileSize;
         offset += frameSize) {
        Frame frame(++frameno, *this);
        if (!frame.initialize()) {
            break;
        }
        m_framePages[frame.getPageNumber()] = frameno;
        m_checksum = frame.getChecksum();
    }
    return true;
}

bool Wal::containsPage(int pageno) const
{
    return m_framePages.find(pageno) != m_framePages.end();
}

Data Wal::acquirePageData(int pageno)
{
    WCTInnerAssert(containsPage(pageno));
    return acquireData(32 + getFrameSize() * m_framePages[pageno] +
                           Frame::headerSize,
                       getPageSize());
}

Data Wal::acquireFrameData(int frameno)
{
    return acquireData(32 + getFrameSize() * frameno, getFrameSize());
}

void Wal::markAsCorrupted()
{
    m_pager.markAsCorrupted();
}

Data Wal::acquireData(off_t offset, size_t size)
{
    if (!m_fileHandle.open(FileHandle::Mode::ReadOnly)) {
        return Data::emptyData();
    }
    Data data(size);
    if (data.empty()) {
        return data;
    }
    ssize_t read = m_fileHandle.read(data.buffer(), offset, size);
    if (read != size) {
        if (read >= 0) {
            //short read
            markAsCorrupted();
        }
        return Data::emptyData();
    }
    return data;
}

int Wal::getPageSize() const
{
    return m_pager.getPageSize();
}

int Wal::getFrameSize() const
{
    return Frame::headerSize + getPageSize();
}

bool Wal::isBigEndian() const
{
    return m_isBigEndian;
}

const std::pair<uint32_t, uint32_t> &Wal::getChecksum() const
{
    return m_checksum;
}

const std::pair<uint32_t, uint32_t> &Wal::getSalt() const
{
    return m_salt;
}

} //namespace Repair

} //namespace WCDB

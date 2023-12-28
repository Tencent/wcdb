//
// Created by sanhuazhang on 2018/05/19
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

#include "Pager.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "FileManager.hpp"
#include "Notifier.hpp"
#include "SQLite.h"
#include "Serialization.hpp"
#include "StringView.hpp"
#include "ThreadedErrors.hpp"
#include <cstring>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Pager::Pager(const UnsafeStringView& path)
: m_fileHandle(path)
, m_pCodec(nullptr)
, m_pageSize(-1)
, m_reservedBytes(-1)
, m_numberOfPages(0)
, m_fileSize(0)
, m_schemaCookie(-1)
, m_wal(this)
, m_walImportance(true)
, m_skipWal(false)
, m_cache(maxAllowedCacheMemory)
, m_highWater(std::make_shared<ShareableHighWater>())
{
}

Pager::~Pager() = default;

void Pager::setPageSize(int pageSize)
{
    WCTAssert(!isInitialized());
    m_pageSize = pageSize;
}

void Pager::setReservedBytes(int reservedBytes)
{
    WCTAssert(!isInitialized());
    m_reservedBytes = reservedBytes;
}

void Pager::setCipherContext(void* ctx)
{
    WCTAssert(ctx != nullptr);
    m_pCodec = ctx;
}

const StringView& Pager::getPath() const
{
    return m_fileHandle.path;
}

#pragma mark - Page
int Pager::getNumberOfPages() const
{
    WCTAssert(isInitialized());
    return std::max(m_wal.getMaxPageno(), m_numberOfPages);
}

int Pager::getUsableSize() const
{
    WCTAssert(isInitialized() || isInitializing());
    return m_pageSize - m_reservedBytes;
}

int Pager::getPageSize() const
{
    WCTAssert(isInitialized() || isInitializing());
    return m_pageSize;
}

int Pager::getReservedBytes() const
{
    WCTAssert(isInitialized());
    return m_reservedBytes;
}

int Pager::getSchemaCookie() const
{
    WCTAssert(isInitialized());
    return m_schemaCookie;
}

UnsafeData Pager::acquirePageData(int number)
{
    return acquirePageData(number, 0, m_pageSize);
}

UnsafeData Pager::acquirePageData(int number, offset_t offset, size_t size)
{
    WCTAssert(isInitialized());
    WCTAssert(number > 0);
    WCTAssert(offset + size <= m_pageSize);
    if (m_cache.exists(number)) {
        return m_cache.get(number).subdata(offset, size);
    }
    UnsafeData data;
    if (m_wal.containsPage(number)) {
        data = m_wal.acquirePageData(number, m_highWater);
    } else {
        if (number > m_numberOfPages) {
            markAsCorrupted(
            number,
            StringView::formatted(
            "Acquired page number: %d exceeds the page count: %d.", number, m_numberOfPages));
            return MappedData::null();
        }
        data = m_fileHandle.mapPage(number, m_highWater);
    }
    if (data.size() != m_pageSize) {
        if (data.size() > 0) {
            //short read
            markAsCorrupted(number,
                            StringView::formatted("Acquired page data with size: %d is less than the expected size: %d.",
                                                  data.size(),
                                                  size));
        } else {
            assignWithSharedThreadedError();
        }
        return MappedData::null();
    }
    if (m_pCodec) {
        void* decodedBuffer = sqlite3Codec(m_pCodec, data.buffer(), number, 4);
        if (decodedBuffer == nullptr) {
            markAsCorrupted(number, "Decode page data fail!");
            return MappedData::null();
        }
        data = Data(reinterpret_cast<unsigned char*>(decodedBuffer), m_pageSize, m_highWater);
    }
    m_cache.insert(number, data);
    tryPurgeCache();
    return data.subdata(offset, size);
}

UnsafeData Pager::acquireHeader()
{
    WCTAssert(m_fileHandle.isOpened());
    WCTAssert(m_pCodec == nullptr || m_pageSize > 0);
    UnsafeData data;
    if (m_pCodec == nullptr) {
        data = m_fileHandle.map(0, 100);
        if (data.size() != 100) {
            assignWithSharedThreadedError();
        }
    } else {
        data = m_fileHandle.map(0, m_pageSize);
        if (data.size() == m_pageSize) {
            void* decodedBuffer = sqlite3Codec(m_pCodec, data.buffer(), 1, 4);
            if (decodedBuffer == nullptr) {
                markAsCorrupted(1, "Decode page data fail!");
                return MappedData::null();
            }
            data
            = Data(reinterpret_cast<unsigned char*>(decodedBuffer), m_pageSize).subdata(0, 100);
        } else {
            assignWithSharedThreadedError();
        }
    }
    if (data.size() != 100) {
        return MappedData::null();
    }
    return data;
}

#pragma mark - Wal
void Pager::setWalImportance(bool flag)
{
    m_walImportance = flag;
    m_wal.setShmLegality(flag);
}

int Pager::getNBackFill() const
{
    return m_wal.getNBackFill();
}

void Pager::setNBackFill(int nbackfill)
{
    m_wal.setNBackFill(nbackfill);
}

const Pager::Salt& Pager::getWalSalt() const
{
    return m_wal.getSalt();
}

void Pager::setWalSalt(const Salt& salt)
{
    return m_wal.setSalt(salt);
}

int Pager::getMaxFrame() const
{
    return m_wal.getMaxFrame();
}

int Pager::getDisposedWalPages() const
{
    return m_wal.getDisposedPages();
}

void Pager::disposeWal()
{
    m_wal.dispose();
}

void Pager::setWalSkipped()
{
    m_skipWal = true;
}

bool Pager::loadWal()
{
    return m_wal.initialize();
}

int Pager::getNumberOfWalFrames() const
{
    return m_wal.getNumberOfFrames();
}

bool Pager::containPageInWal(uint32_t pageno)
{
    return m_wal.containsPage(pageno);
}

#pragma mark - Error
void Pager::markAsCorrupted(int page, const UnsafeStringView& message)
{
    Error error(Error::Code::Corrupt, Error::Level::Notice, message);
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, getPath());
    error.infos.insert_or_assign("Page", page);
    Notifier::shared().notify(error);
    setError(std::move(error));
}

void Pager::markAsError(Error::Code code)
{
    Error error(code, Error::Level::Notice);
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, getPath());
    Notifier::shared().notify(error);
    setError(std::move(error));
}

#pragma mark - Initializeable
bool Pager::doInitialize()
{
    auto fileSize = FileManager::getFileSize(getPath());
    if (!fileSize.succeed()) {
        assignWithSharedThreadedError();
        return false;
    }
    m_fileSize = fileSize.value();
    if (m_fileSize == 0) {
        markAsError(Error::Code::Empty);
        return false;
    }

    if (!m_fileHandle.open(FileHandle::Mode::ReadOnly)) {
        assignWithSharedThreadedError();
        return false;
    }
    FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(getPath());
    if (m_pageSize == -1 || m_reservedBytes == -1) {
        UnsafeData data = acquireHeader();
        if (data.empty()) {
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
            WCTAssert(deserialization.canAdvance(2));
            m_pageSize = deserialization.advance2BytesInt();
        }
        //parse reserved bytes
        if (m_reservedBytes == -1) {
            deserialization.seek(20);
            WCTAssert(deserialization.canAdvance(1));
            m_reservedBytes = deserialization.advance1ByteInt();
        }
        //parse schema cookie
        if (m_schemaCookie == -1) {
            deserialization.seek(40);
            WCTAssert(deserialization.canAdvance(4));
            m_schemaCookie = deserialization.advance4BytesInt();
        }
    }
    if (((m_pageSize - 1) & m_pageSize) != 0 || m_pageSize < 512 || m_pageSize > 65536) {
        markAsCorrupted(
        1, StringView::formatted("Page size: %d is not aligned or not too small.", m_pageSize));
        return false;
    }
    if (m_reservedBytes < 0 || m_reservedBytes > 255) {
        markAsCorrupted(
        1, StringView::formatted("Reversed bytes: %d is illegal.", m_reservedBytes));
        return false;
    }

    m_fileHandle.setPageSize(m_pageSize);

    m_numberOfPages = (int) ((m_fileSize + m_pageSize - 1) / m_pageSize);

    if (m_skipWal) {
        return true;
    }

    if (m_wal.initialize()) {
        return true;
    }
    if (m_walImportance || !m_error.isCorruption()) {
        return false;
    }
    disposeWal();
    return true;
}

void Pager::tryPurgeCache()
{
    ssize_t allowedSize = maxAllowedCacheMemory * 2;
    if (m_pCodec) {
        allowedSize *= 2;
    }
    if (m_highWater->getCurrent() <= allowedSize) {
        return;
    }
    if (m_pCodec != nullptr) {
        m_fileHandle.purgeAll();
        while (!m_cache.empty() && m_highWater->getCurrent() > allowedSize) {
            m_cache.purge();
        }
    } else {
        m_cache.purge(m_cache.size());
        while (m_highWater->getCurrent() > allowedSize && m_fileHandle.purgeOne())
            ;
    }
}

Pager::Cache::Cache(size_t maxAllowedMemory)
: LRUCache<uint32_t, UnsafeData>()
, m_maxAllowedMemory(maxAllowedMemory)
, m_currentUsedMemery(0)
{
}

Pager::Cache::~Cache() = default;

void Pager::Cache::insert(uint32_t pageNum, const UnsafeData& data)
{
    WCTAssert(!exists(pageNum));
    m_currentUsedMemery += data.size();
    put(pageNum, data);
}

bool Pager::Cache::shouldPurge() const
{
    return m_currentUsedMemery > m_maxAllowedMemory;
}

void Pager::Cache::willPurge(const uint32_t& pageNum, const UnsafeData& data)
{
    WCDB_UNUSED(pageNum)
    m_currentUsedMemery -= data.size();
}

} //namespace Repair

} //namespace WCDB

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

#ifndef __WCDB_PAGER_HPP
#define __WCDB_PAGER_HPP

#include <WCDB/Error.hpp>
#include <WCDB/ErrorProne.hpp>
#include <WCDB/Initializeable.hpp>
#include <WCDB/PageBasedFileHandle.hpp>
#include <WCDB/Wal.hpp>

namespace WCDB {

class Data;

namespace Repair {

class Pager final : public ErrorProne, public Initializeable {
#pragma mark - Initialize
public:
    Pager(const String &path);

    void setPageSize(int pageSize);
    void setReservedBytes(int reservedBytes);

    const String &getPath() const;

protected:
    PageBasedFileHandle m_fileHandle;
    friend class PagerRelated;

#pragma mark - Page
public:
    int getNumberOfPages() const;
    MappedData acquirePageData(int number);
    MappedData acquirePageData(int number, off_t offset, size_t size);

    int getUsableSize() const;
    int getPageSize() const;
    int getReservedBytes() const;

protected:
    MappedData acquireData(off_t offset, size_t size);
    int m_pageSize;
    int m_reservedBytes;
    int m_numberOfPages;
    size_t m_fileSize;

#pragma mark - Wal
public:
    void setWalImportance(bool flag);
    int getNumberOfWalFrames() const;
    void setMaxWalFrame(int maxWalFrame);
    int getDisposedWalPages() const;
    void disposeWal();
    const std::pair<uint32_t, uint32_t> &getWalSalt() const;

protected:
    Wal m_wal;
    bool m_walImportance;

#pragma mark - Error
public:
    void markAsCorrupted(int page, const String &message);
    void hint() const;

protected:
    void markAsError(Error::Code code);

#pragma mark - Initializeable
protected:
    bool doInitialize() override final;
};

} //namespace Repair

} //namespace WCDB

#endif /* __WCDB_PAGER_HPP */

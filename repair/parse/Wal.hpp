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

#ifndef Wal_hpp
#define Wal_hpp

#include <WCDB/FileHandle.hpp>
#include <WCDB/Initializeable.hpp>
#include <WCDB/PagerRelated.hpp>
#include <map>
#include <set>

namespace WCDB {

namespace Repair {

class Wal : public PagerRelated, public Initializeable {
#pragma mark - Initialize
public:
    Wal(Pager *pager);

    const std::string &getPath() const;
    static constexpr const int headerSize = 32;

protected:
    FileHandle m_fileHandle;
    friend class WalRelated;
    MappedData acquireData(off_t offset, size_t size);

#pragma mark - Page
public:
    bool containsPage(int pageno) const;
    MappedData acquirePageData(int pageno);
    MappedData acquirePageData(int pageno, off_t offset, size_t size);
    int getMaxPageno() const;

protected:
    // pageno -> frameno
    std::map<int, int> m_framePages;

#pragma mark - Wal
public:
    void setMaxAllowedFrame(int maxFrame);
    int getFrameCount() const;
    bool isNativeChecksum() const;
    const std::pair<uint32_t, uint32_t> &getSalt() const;
    int getPageSize() const;

protected:
    std::pair<uint32_t, uint32_t>
    calculateChecksum(const MappedData &data, const std::pair<uint32_t, uint32_t> &checksum);

    int m_maxAllowedFrame;
    int m_maxFrames;
    bool m_isNativeChecksum;
    std::pair<uint32_t, uint32_t> m_salt;

#pragma mark - Shm
public:
    typedef struct {
        uint32_t version;
        uint32_t unused;
        uint32_t change;
        uint8_t isInit;
        uint8_t bigEndChecksum;
        uint16_t pageSize;
        uint32_t maxFrame;
        uint32_t pageCount;
        uint32_t frameChecksum[2];
        uint32_t salt[2];
        uint32_t ckecksum[2];
    } ShmHeader;

#pragma mark - Frame
public:
    int getFrameSize() const;
    MappedData acquireFrameData(int frameno);

#pragma mark - Initializeable
protected:
    bool doInitialize() override;
    static bool isBigEndian();

#pragma mark - Error
protected:
    void markAsCorrupted(int frame, const std::string &message);
    void markAsError(Error::Code code);

#pragma mark - Dispose
public:
    int getDisposedPage() const;
    void dispose();

protected:
    std::set<int> m_disposedPages;
};

} //namespace Repair

} //namespace WCDB

#endif /* Wal_hpp */

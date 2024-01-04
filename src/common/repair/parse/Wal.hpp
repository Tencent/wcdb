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

#pragma once

#include "FileHandle.hpp"
#include "Initializeable.hpp"
#include "PagerRelated.hpp"
#include "Shm.hpp"
#include <map>
#include <set>

namespace WCDB {

namespace Repair {

class Wal final : public PagerRelated, public Initializeable {
#pragma mark - Initialize
public:
    Wal(Pager *pager);
    ~Wal() override;

    const StringView &getPath() const;
    static constexpr const int headerSize = 32;

protected:
    FileHandle m_fileHandle;
    friend class WalRelated;
    MappedData
    acquireData(offset_t offset, size_t size, SharedHighWater highWater = nullptr);

#pragma mark - Page
public:
    bool containsPage(int pageno) const;
    MappedData acquirePageData(int pageno, SharedHighWater highWater = nullptr);
    MappedData
    acquirePageData(int pageno, offset_t offset, size_t size, SharedHighWater highWater = nullptr);
    int getMaxPageno() const;

protected:
    // pageno -> frameno
    std::map<int, int> m_pages2Frames;

#pragma mark - Wal
public:
    typedef Shm::Salt Salt;
    static bool isCheckpointIncreasedSalt(const Salt &before, const Salt &after);

    void setShmLegality(bool flag);
    int getNBackFill() const;
    void setNBackFill(int nbackfill);
    int getNumberOfFrames() const;
    const Salt &getSalt() const;
    void setSalt(const Salt &salt);
    int getMaxFrame() const;
    int getPageSize() const;

protected:
    std::pair<uint32_t, uint32_t>
    calculateChecksum(const MappedData &data,
                      const std::pair<uint32_t, uint32_t> &checksum) const;

    size_t m_fileSize;
    uint32_t m_truncate;
    int m_nbackfill;
    int m_maxFrame;
    bool m_isNativeChecksum;
    Salt m_salt;
    bool m_shmLegality;
    Shm m_shm;

#pragma mark - Frame
public:
    int getFrameSize() const;
    MappedData acquireFrameData(int frameno);

#pragma mark - Initializeable
protected:
    bool doInitialize() override final;
    static bool isBigEndian();

#pragma mark - Error
protected:
    void markAsCorrupted(int frame, const UnsafeStringView &message);
    //    void markAsError(Error::Code code);

#pragma mark - Dispose
public:
    int getDisposedPages() const;
    void dispose();

protected:
    std::set<int> m_disposedPages;
};

} //namespace Repair

} //namespace WCDB

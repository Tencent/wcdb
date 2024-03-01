//
// Created by sanhuazhang on 2018/07/23
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
#include "LRUCache.hpp"
#include "MappedData.hpp"
#include "Range.hpp"

namespace WCDB {

class PageBasedFileHandle final : public FileHandle {
#pragma mark - PageBasedFileHandle
public:
    PageBasedFileHandle(const UnsafeStringView& path);
    ~PageBasedFileHandle() override;

    MappedData
    mapPage(int pageno, offset_t offset, size_t size, SharedHighWater highWater = nullptr);
    MappedData mapPage(int pageno, SharedHighWater highWater = nullptr);

protected:
    static Range
    restrictedRange(Range::Location base, Range::Length maxLength, const Range& restrictor);

#pragma mark - PageSize
public:
    void setPageSize(size_t pageSize);
    size_t cachePagePerRange() const;

protected:
    size_t m_pageSize;

#pragma mark - Cache
public:
    void purgeAll();
    bool purgeOne();

protected:
    static constexpr const size_t cacheMemoryPerRange = 1 * 1024 * 1024;
    static constexpr const size_t maxAllowedCacheMemory = 16 * 1024 * 1024;

    class Cache final : protected LRUCache<Range, MappedData> {
    public:
        Cache(size_t maxAllowedMemory);
        ~Cache() override;

        using Super = LRUCache<Range, MappedData>;
        using Location = Range::Location;
        using Length = Range::Length;
        using Super::purge;
        using Super::size;
        using Super::empty;

        void setRange(const Range& range);
        std::pair<Range, const MappedData*> find(Location location);
        void insert(const Range& range, const MappedData& data);

    protected:
        MapIterator findIterator(Location location);
        Range m_range;

        bool shouldPurge() const override final;
        void willPurge(const Range& range, const MappedData& data) override final;
        size_t m_maxAllowedMemory;
        size_t m_currentUsedMemery;
    };

    Cache m_cache;
    size_t m_cachePageSize;
};

} // namespace WCDB

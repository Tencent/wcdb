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

#ifndef PageBasedFileHandle_hpp
#define PageBasedFileHandle_hpp

#include <WCDB/FileHandle.hpp>
#include <WCDB/LRUCache.hpp>
#include <WCDB/MappedData.hpp>
#include <WCDB/Range.hpp>

namespace WCDB {

class PageBasedFileHandle : public FileHandle {
#pragma mark - PageBasedFileHandle
public:
    PageBasedFileHandle(const std::string& path);

    MappedData mapPage(int pageno, off_t offset, size_t size);
    MappedData mapPage(int pageno);

#pragma mark - PageSize
public:
    void setPageSize(size_t pageSize);

protected:
    size_t m_pageSize;

#pragma mark - Cache
protected:
    static constexpr const int maxMapPage = 8 * 1024;
    static constexpr const int maxCacheSize = 5;

    class Cache : protected LRUCache<Range, MappedData> {
    public:
        Cache(size_t maxSize);

        using Super = LRUCache<Range, MappedData>;
        using Location = Range::Location;
        using Length = Range::Length;
        using Super::purge;
        using Super::empty;

        void setRange(const Range& range);
        std::pair<Range, const MappedData*> find(Location location);
        void insert(const Range& range, const MappedData& data);

    protected:
        MapIterator findIterator(Location location);
        Range m_range;
    };

    Cache m_cache;
    size_t m_cachePageSize;
};

} // namespace WCDB

#endif /* PageBasedFileHandle_hpp */

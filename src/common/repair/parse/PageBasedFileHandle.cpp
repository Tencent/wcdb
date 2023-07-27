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

#include "PageBasedFileHandle.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "Notifier.hpp"
#include "WCDBError.hpp"

namespace WCDB {

#pragma mark - PageBasedFileHandle
PageBasedFileHandle::PageBasedFileHandle(const UnsafeStringView& path)
: FileHandle(path), m_pageSize(0), m_cache(maxAllowedCacheMemory), m_cachePageSize(0)
{
    static_assert(maxAllowedCacheMemory % cacheMemoryPerRange == 0, "");
    static_assert((maxAllowedCacheMemory & maxAllowedCacheMemory - 1) == 0, "");
    static_assert((cacheMemoryPerRange & cacheMemoryPerRange - 1) == 0, "");
}

PageBasedFileHandle::~PageBasedFileHandle() = default;

Range PageBasedFileHandle::restrictedRange(Range::Location base,
                                           Range::Length maxLength,
                                           const Range& restrictor)
{
    Range result;
    result.length = std::min(maxLength, restrictor.length);
    result.location = base - Range::Location(result.length / 2);
    if (result.location < restrictor.location) {
        result.shiftToLocation(restrictor.location);
    } else if (result.edge() > restrictor.edge()) {
        result.shiftToEdge(restrictor.edge());
    }
    return result;
}

MappedData PageBasedFileHandle::mapPage(int pageno,
                                        offset_t offsetWithinPage,
                                        size_t sizeWithinPage,
                                        SharedHighWater highWater)
{
    WCTAssert(m_cachePageSize > 0);
    WCTAssert(pageno > 0);
    WCTAssert(offsetWithinPage < m_pageSize);
    WCTAssert(sizeWithinPage <= m_pageSize && sizeWithinPage > 0);
    WCTAssert(m_cachePageSize >= m_pageSize);

    offset_t offset = (pageno - 1) * m_pageSize + offsetWithinPage;
    offset_t cachePageno = offset / m_cachePageSize;

    // assert same cache page
    WCTAssert(cachePageno == (offset + sizeWithinPage - 1) / m_cachePageSize);

    Range gap;
    const MappedData* cachedData;
    std::tie(gap, cachedData) = m_cache.find(cachePageno);
    if (cachedData != nullptr) {
        WCTAssert(gap.contains(cachePageno));
        offset_t offsetWithinCache = offset - gap.location * m_cachePageSize;
        WCTAssert(offsetWithinCache < gap.length * m_cachePageSize);
        return cachedData->subdata(offsetWithinCache, sizeWithinPage);
    }

    Range::Length maxLength = cachePagePerRange();
    do {
        Range range = restrictedRange(cachePageno, maxLength, gap);

        WCTAssert(gap.contains(range));
        WCTAssert(range.contains(cachePageno));

        bool cuttable = range.length > 1;
        bool purgeable = !m_cache.empty();
        bool ignorable = cuttable || purgeable;

        markErrorAsIgnorable(ignorable);
        MappedData mappedData
        = map(range.location * m_cachePageSize, range.length * m_cachePageSize, highWater);
        markErrorAsIgnorable(false);

        if (!mappedData.empty()) {
            m_cache.insert(range, mappedData);
            offset_t offsetWithinCache = offset - range.location * m_cachePageSize;
            WCTAssert(offsetWithinCache < range.length * m_cachePageSize);
            return mappedData.subdata(offsetWithinCache, sizeWithinPage);
        } else if (cuttable) {
            maxLength = (range.length + 1) / 2;
            WCTAssert(maxLength >= 1);
        } else if (purgeable) {
            m_cache.purge();
            std::tie(gap, cachedData) = m_cache.find(cachePageno);
            maxLength = cachePagePerRange();
            WCTAssert(cachedData == nullptr);
        } else {
            break;
        }
    } while (true);
    return MappedData::null();
}

#pragma mark - PageSize
MappedData PageBasedFileHandle::mapPage(int pageno, SharedHighWater highWater)
{
    WCTAssert(pageno > 0);
    return mapPage(pageno, 0, m_pageSize, highWater);
}

size_t PageBasedFileHandle::cachePagePerRange() const
{
    WCTAssert(m_cachePageSize != 0);
    return cacheMemoryPerRange / m_cachePageSize;
}

void PageBasedFileHandle::setPageSize(size_t pageSize)
{
    WCTAssert(m_cachePageSize == 0);
    m_pageSize = pageSize;
    m_cachePageSize = pageSize;
    size_t alignment = m_cachePageSize % memoryPageSize();
    if (alignment > 0) {
        m_cachePageSize = m_cachePageSize - alignment + memoryPageSize();
    }
    WCTAssert(m_cachePageSize > 0 && m_cachePageSize % memoryPageSize() == 0);
    m_cachePageSize = std::max(memoryPageSize(), m_cachePageSize);
    WCTAssert(m_cachePageSize > 0 && cacheMemoryPerRange % m_cachePageSize == 0);

    size_t fileSize = FileHandle::size();
    Range::Length restrictCachePageno
    = fileSize / m_cachePageSize + (fileSize % m_cachePageSize > 0);
    m_cache.setRange(Range(0, restrictCachePageno));
}

#pragma mark - Cache
void PageBasedFileHandle::purgeAll()
{
    m_cache.purge(m_cache.size());
}

bool PageBasedFileHandle::purgeOne()
{
    if (m_cache.empty()) {
        return false;
    }
    m_cache.purge();
    return true;
}

PageBasedFileHandle::Cache::Cache(size_t maxAllowedMemory)
: LRUCache<WCDB::Range, WCDB::MappedData>()
, m_range(Range::notFound())
, m_maxAllowedMemory(maxAllowedMemory)
, m_currentUsedMemery(0)
{
}

PageBasedFileHandle::Cache::~Cache() = default;

void PageBasedFileHandle::Cache::setRange(const WCDB::Range& range)
{
    WCTAssert(range != Range::notFound());
    m_range = range;
}

std::pair<Range, const MappedData*> PageBasedFileHandle::Cache::find(Location location)
{
    WCTAssert(m_range != Range::notFound());
    WCTAssert(m_range.contains(location));

    Range range;
    const MappedData* data = nullptr;

    // assume: a < b < c < d < e < f < g
    // { [b, c), [e, f) }
    // findIterator(a) = [b, c), find(a) = [min, b)
    // findIterator(b) = [b, c), find(b) = [b, c)
    // findIterator(c) = [e, f), find(c) = [c, e)
    // findIterator(d) = [e, f), find(d) = [c, e)
    // findIterator(e) = [e, f), find(e) = [e, f)
    // findIterator(f) = end(), find(f) = [f, max)
    // findIterator(g) = end(), find(g) = [f, max)
    auto match = findIterator(location);
    if (match != m_map.end()) {
        if (match->first.contains(location)) {
            retain(match);
            range = match->first;
            data = &(match->second->second);
        } else {
            if (match != m_map.begin()) {
                auto previous = match;
                --previous;
                range.location = previous->first.edge();
            } else {
                range.location = m_range.location;
            }
            range.expandToEdge(match->first.location);
        }
    } else {
        if (!m_map.empty()) {
            auto last = m_map.end();
            --last;
            range.location = last->first.edge();
        } else {
            range.location = m_range.location;
        }
        range.expandToEdge(m_range.edge());
    }
    return std::make_pair(range, data);
}

void PageBasedFileHandle::Cache::insert(const Range& range, const MappedData& data)
{
    WCTAssert(range.length > 0);
    WCTAssert(m_range.contains(range));
    WCTAssert(find(range.location).second == nullptr);
    WCTAssert(find(range.edge() - 1).second == nullptr);
    m_currentUsedMemery += data.size();
    put(range, data);
}

bool PageBasedFileHandle::Cache::shouldPurge() const
{
    return m_currentUsedMemery > m_maxAllowedMemory;
}

void PageBasedFileHandle::Cache::willPurge(const Range& range, const MappedData& data)
{
    WCDB_UNUSED(range)
    m_currentUsedMemery -= data.size();
}

PageBasedFileHandle::Cache::MapIterator
PageBasedFileHandle::Cache::findIterator(Location location)
{
    return std::find_if(m_map.begin(), m_map.end(), [&location](const auto& iter) -> bool {
        const Range& range = iter.first;
        return location < range.edge();
    });
}

} // namespace WCDB

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
#include <WCDB/Error.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/PageBasedFileHandle.hpp>
#include <unistd.h>

namespace WCDB {

#pragma mark - PageBasedFileHandle
PageBasedFileHandle::PageBasedFileHandle(const String& path)
: FileHandle(path), m_pageSize(0), m_cachePageSize(0), m_cache(maxAllowedCacheMemory)
{
    static_assert(maxAllowedCacheMemory % cacheMemoryPerRange == 0, "");
    static_assert((maxAllowedCacheMemory & maxAllowedCacheMemory - 1) == 0, "");
    static_assert((cacheMemoryPerRange & cacheMemoryPerRange - 1) == 0, "");
}

Range PageBasedFileHandle::restrictedRange(Range::Location base,
                                           Range::Length maxLength,
                                           const Range& restrictor)
{
    Range result;
    result.length = std::min(maxLength, restrictor.length);
    result.location = base - result.length / 2;
    if (result.location < restrictor.location) {
        result.shiftToLocation(restrictor.location);
    } else if (result.edge() > restrictor.edge()) {
        result.shiftToEdge(restrictor.edge());
    }
    return result;
}

MappedData PageBasedFileHandle::mapPage(int pageno, off_t offsetWithinPage, size_t sizeWithinPage)
{
    WCTInnerAssert(m_cachePageSize > 0);
    WCTInnerAssert(pageno > 0);
    WCTInnerAssert(offsetWithinPage < m_pageSize);
    WCTInnerAssert(sizeWithinPage <= m_pageSize && sizeWithinPage > 0);
    WCTInnerAssert(m_cachePageSize >= m_pageSize);

    off_t offset = (pageno - 1) * m_pageSize + offsetWithinPage;
    off_t cachePageno = offset / m_cachePageSize;

    // assert same cache page
    WCTInnerAssert(cachePageno == (offset + sizeWithinPage - 1) / m_cachePageSize);

    Range gap;
    const MappedData* cachedData;
    std::tie(gap, cachedData) = m_cache.find(cachePageno);
    if (cachedData != nullptr) {
        WCTInnerAssert(gap.contains(cachePageno));
        off_t offsetWithinCache = offset - gap.location * m_cachePageSize;
        WCTInnerAssert(offsetWithinCache < gap.length * m_cachePageSize);
        return cachedData->subdata(offsetWithinCache, sizeWithinPage);
    }

    Range::Length maxLength = cachePagePerRange();
    do {
        Range range = restrictedRange(cachePageno, maxLength, gap);

        WCTInnerAssert(gap.contains(range));
        WCTInnerAssert(range.contains(cachePageno));

        bool cuttable = range.length > 1;
        bool purgeable = !m_cache.empty();
        bool ignorable = cuttable || purgeable;

        markErrorAsIgnorable(ignorable);
        MappedData mappedData
        = map(range.location * m_cachePageSize, range.length * m_cachePageSize);
        markErrorAsIgnorable(false);

        if (!mappedData.empty()) {
            ssize_t highWater = MappedData::getMappedHighWater();
            static constexpr const ssize_t s_allowedHighWater = maxAllowedCacheMemory * 2;
            if (highWater > s_allowedHighWater) {
                Error error;
                error.level = Error::Level::Warning;
                error.setCode(Error::Code::Warning, "Repair");
                error.message = "Mapped memory exceeds.";
                error.infos.set("Path", path);
                error.infos.set("HighWater", highWater);
                error.infos.set("AllowedHighWater", s_allowedHighWater);
                Notifier::shared()->notify(error);

                while (!m_cache.empty() && MappedData::getMappedHighWater() > s_allowedHighWater) {
                    m_cache.purge();
                }
            }

            m_cache.insert(range, mappedData);
            off_t offsetWithinCache = offset - range.location * m_cachePageSize;
            WCTInnerAssert(offsetWithinCache < range.length * m_cachePageSize);
            return mappedData.subdata(offsetWithinCache, sizeWithinPage);
        } else if (cuttable) {
            maxLength = (range.length + 1) / 2;
            WCTInnerAssert(maxLength >= 1);
        } else if (purgeable) {
            m_cache.purge();
            std::tie(gap, cachedData) = m_cache.find(cachePageno);
            maxLength = cachePagePerRange();
            WCTInnerAssert(cachedData == nullptr);
        } else {
            break;
        }
    } while (true);
    return MappedData::null();
}

#pragma mark - PageSize
MappedData PageBasedFileHandle::mapPage(int pageno)
{
    WCTInnerAssert(pageno > 0);
    return mapPage(pageno, 0, m_pageSize);
}

const size_t& PageBasedFileHandle::memoryPageSize()
{
    static size_t s_memoryPageSize = getpagesize();
    return s_memoryPageSize;
}

size_t PageBasedFileHandle::cachePagePerRange() const
{
    WCTInnerAssert(m_cachePageSize != 0);
    return cacheMemoryPerRange / m_cachePageSize;
}

void PageBasedFileHandle::setPageSize(size_t pageSize)
{
    WCTInnerAssert(m_cachePageSize == 0);
    m_pageSize = pageSize;
    m_cachePageSize = pageSize;
    size_t alignment = m_cachePageSize % memoryPageSize();
    if (alignment > 0) {
        m_cachePageSize = m_cachePageSize - alignment + memoryPageSize();
    }
    WCTInnerAssert(m_cachePageSize > 0 && m_cachePageSize % memoryPageSize() == 0);
    m_cachePageSize = std::max(memoryPageSize(), m_cachePageSize);
    WCTInnerAssert(cacheMemoryPerRange % m_cachePageSize == 0);

    size_t fileSize = FileHandle::size();
    Range::Length restrictCachePageno
    = fileSize / m_cachePageSize + (fileSize % m_cachePageSize > 0);
    m_cache.setRange(Range(0, restrictCachePageno));
}

#pragma mark - Cache
PageBasedFileHandle::Cache::Cache(size_t maxAllowedMemory)
: LRUCache<WCDB::Range, WCDB::MappedData>()
, m_range(Range::notFound())
, m_maxAllowedMemory(maxAllowedMemory)
, m_currentUsedMemery(0)
{
}

void PageBasedFileHandle::Cache::setRange(const WCDB::Range& range)
{
    WCTInnerAssert(range != Range::notFound());
    m_range = range;
}

std::pair<Range, const MappedData*> PageBasedFileHandle::Cache::find(Location location)
{
    WCTInnerAssert(m_range != Range::notFound());
    WCTInnerAssert(m_range.contains(location));

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
    WCTInnerAssert(range.length > 0);
    WCTInnerAssert(m_range.contains(range));
    WCTInnerAssert(find(range.location).second == nullptr);
    WCTInnerAssert(find(range.edge() - 1).second == nullptr);
    m_currentUsedMemery += data.size();
    put(range, data);
}

bool PageBasedFileHandle::Cache::shouldPurge() const
{
    return m_currentUsedMemery > m_maxAllowedMemory;
}

void PageBasedFileHandle::Cache::willPurge(const Range& range, const MappedData& data)
{
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

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
#include <WCDB/PageBasedFileHandle.hpp>
#include <unistd.h>

namespace WCDB {

#pragma mark - PageBasedFileHandle
PageBasedFileHandle::PageBasedFileHandle(const std::string& path)
: FileHandle(path), m_pageSize(0), m_cachePageSize(0)
{
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

    Range range;
    const MappedData* cachedData;
    std::tie(range, cachedData) = m_cache.find(cachePageno);
    if (cachedData != nullptr) {
        WCTInnerAssert(range.contains(cachePageno));
        off_t offsetWithinCache = offset - range.location * m_cachePageSize;
        WCTInnerAssert(offsetWithinCache < range.length * m_cachePageSize);
        return cachedData->subdata(offsetWithinCache, sizeWithinPage);
    }

    size_t fileSize = FileHandle::size();
    Range::Location restrictCachePageno
    = fileSize / m_cachePageSize + (fileSize % m_cachePageSize > 0);
    do {
        range.restrict_(0, restrictCachePageno);

        bool cuttable = range.length > 1;
        bool purgeable = !m_cache.empty();
        bool ignorable = cuttable || purgeable;

        markErrorAsIgnorable(ignorable);
        MappedData mappedData
        = map(range.location * m_cachePageSize, range.length * m_cachePageSize);
        markErrorAsIgnorable(false);

        if (!mappedData.empty()) {
            WCTInnerAssert(range.contains(cachePageno));
            m_cache.insert(range, mappedData);
            off_t offsetWithinCache = offset - range.location * m_cachePageSize;
            WCTInnerAssert(offsetWithinCache < range.length * m_cachePageSize);
            return mappedData.subdata(offsetWithinCache, sizeWithinPage);
        } else if (cuttable) {
            range.location = (range.location + 1 + cachePageno) / 2;
            range.setEdge((range.edge() + 1 + cachePageno) / 2);
            WCTInnerAssert(range.length >= 1);
        } else if (purgeable) {
            m_cache.purge();
            std::tie(range, cachedData) = m_cache.find(cachePageno);
            WCTInnerAssert(cachedData == nullptr);
        } else {
            break;
        }
    } while (true);
    return MappedData::emptyData();
}

#pragma mark - PageSize
MappedData PageBasedFileHandle::mapPage(int pageno)
{
    WCTInnerAssert(pageno > 0);
    return mapPage(pageno, 0, m_pageSize);
}

void PageBasedFileHandle::setPageSize(size_t pageSize)
{
    WCTInnerAssert(m_cachePageSize == 0);
    m_pageSize = pageSize;
    static size_t s_memoryPageSize = getpagesize();
    m_cachePageSize = pageSize;
    size_t alignment = m_cachePageSize % s_memoryPageSize;
    if (alignment > 0) {
        m_cachePageSize = m_cachePageSize - alignment + s_memoryPageSize;
    }
    WCTInnerAssert(m_cachePageSize > 0 && m_cachePageSize % s_memoryPageSize == 0);
    m_cachePageSize = std::max(s_memoryPageSize, m_cachePageSize);
}

#pragma mark - Cache
std::pair<Range, const MappedData*> PageBasedFileHandle::Cache::find(Location location)
{
    static constexpr const auto s_min = std::numeric_limits<Location>::min();
    static constexpr const auto s_max = std::numeric_limits<Location>::max();

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
                range.location = s_min;
            }
            range.setEdge(match->first.location);
        }
    } else {
        if (!m_map.empty()) {
            auto last = m_map.end();
            --last;
            range.location = last->first.edge();
        } else {
            range.location = s_min;
        }
        range.setEdge(s_max);
    }
    return std::make_pair(range, data);
}

void PageBasedFileHandle::Cache::insert(const Range& range, const MappedData& data)
{
    assert(range.length > 0);
    assert(find(range.location).second == nullptr);
    assert(find(range.edge() - 1).second == nullptr);
    put(range, data);
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

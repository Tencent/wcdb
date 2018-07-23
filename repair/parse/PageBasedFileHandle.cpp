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

PageBasedFileHandle::PageBasedFileHandle(const std::string& path)
: FileHandle(path), m_pageSize(0), m_cachePageSize(0)
{
}

void PageBasedFileHandle::setPageSize(size_t pageSize)
{
    WCTInnerAssert(m_cachePageSize == 0);
    m_pageSize = pageSize;
    static size_t s_memoryPageSize = getpagesize();
    size_t cachePageSize = pageSize;
    size_t alignment = cachePageSize % s_memoryPageSize;
    if (alignment > 0) {
        cachePageSize = cachePageSize - alignment + s_memoryPageSize;
    }
    WCTInnerAssert(cachePageSize > 0 && cachePageSize % s_memoryPageSize == 0);
    m_cachePageSize = std::max(s_memoryPageSize, cachePageSize);
}

MappedData PageBasedFileHandle::lazyMap(int pageno, off_t offset, size_t size)
{
    WCTInnerAssert(m_cachePageSize > 0);
    WCTInnerAssert(pageno > 0);
    WCTInnerAssert(offset < m_pageSize);
    WCTInnerAssert(size <= m_pageSize && size > 0);
    off_t resolvedOffset = (pageno - 1) * m_pageSize;
    int cachePageno = (int) resolvedOffset / m_cachePageSize;
    off_t resolvedOffsetInCache = resolvedOffset - cachePageno * m_cachePageSize;
    if (m_cache.exists(cachePageno)) {
        return m_cache.get(cachePageno).subdata(resolvedOffsetInCache, size);
    }
    do {
        bool purgeable = !m_cache.empty();
        markErrorAsIgnorable(purgeable);
        MappedData data = map(cachePageno * m_cachePageSize, m_cachePageSize);
        markErrorAsIgnorable(false);
        if (!data.empty()) {
            m_cache.put(cachePageno, data);
            return data.subdata(resolvedOffsetInCache, size);
        } else if (purgeable) {
            m_cache.purge();
        } else {
            break;
        }
    } while (true);
    return MappedData::emptyData();
}

MappedData PageBasedFileHandle::lazyMap(int pageno)
{
    WCTInnerAssert(pageno > 0) return lazyMap(pageno, 0, m_pageSize);
}

} // namespace WCDB

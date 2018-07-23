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

namespace WCDB {

class PageBasedFileHandle : public FileHandle {
public:
    PageBasedFileHandle(const std::string& path);

    void setPageSize(size_t pageSize);

    MappedData mapPage(int pageno, off_t offset, size_t size);
    MappedData mapPage(int pageno);

protected:
    LRUCache<int, MappedData> m_cache;
    size_t m_cachePageSize;

    size_t m_pageSize;
};

} // namespace WCDB

#endif /* PageBasedFileHandle_hpp */

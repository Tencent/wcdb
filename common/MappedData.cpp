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

#include <WCDB/MappedData.hpp>
#include <sys/mman.h>

namespace WCDB {

MappedData::MappedData()
: UnsafeData(), m_mapped(UnsafeData::emptyData(), nullptr)
{
}

MappedData::MappedData(unsigned char* mapped, size_t size)
: UnsafeData(mapped, size)
, m_mapped(UnsafeData(mapped, size),
           [](UnsafeData& data) { munmap(data.buffer(), data.size()); })
{
}

MappedData::MappedData(const UnsafeData& data, const Recyclable<UnsafeData>& mapped)
: UnsafeData(data), m_mapped(mapped)
{
}

MappedData MappedData::subdata(size_t size) const
{
    return MappedData(UnsafeData::subdata(size), m_mapped);
}

MappedData MappedData::subdata(off_t offset, size_t size) const
{
    return MappedData(UnsafeData::subdata(offset, size), m_mapped);
}

const MappedData& MappedData::emptyData()
{
    static const MappedData* s_empty = new MappedData;
    return *s_empty;
}

} // namespace WCDB

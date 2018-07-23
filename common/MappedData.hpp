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

#ifndef MappedData_hpp
#define MappedData_hpp

#include <WCDB/Recyclable.hpp>
#include <WCDB/UnsafeData.hpp>

namespace WCDB {

class MappedData : public UnsafeData {
public:
    MappedData();
    MappedData(unsigned char* mapped, size_t size);

    MappedData subdata(size_t size) const;
    MappedData subdata(off_t offset, size_t size) const;

    static const MappedData& emptyData();

protected:
    MappedData(const UnsafeData& data, const Recyclable<UnsafeData>& mapped);
    Recyclable<UnsafeData> m_mapped;
};

} // namespace WCDB

#endif /* MappedData_hpp */

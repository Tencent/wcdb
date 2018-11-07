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

#ifndef _WCDB_DATA_HPP
#define _WCDB_DATA_HPP

#include <WCDB/UnsafeData.hpp>
#include <memory>
#include <vector>

namespace WCDB {

class Data final : public UnsafeData {
#pragma mark - Initialize
public:
    Data();
    Data(const UnsafeData& data);
    Data(const Data& data);
    Data(Data&& data);
    Data(size_t size);
    Data(const unsigned char* buffer, size_t size);

    Data& operator=(const Data& other);
    Data& operator=(Data&& other);

protected:
    Data(const std::shared_ptr<unsigned char>& sharedBuffer, size_t sharedSize, off_t offset, size_t size);
    off_t getCurrentOffset() const;
    size_t getSharedSize() const;

private:
    std::shared_ptr<unsigned char> m_sharedBuffer;
    size_t m_sharedSize;

#pragma mark - Reset
public:
    bool resize(size_t size);

    bool reset(const unsigned char* buffer, size_t size);
    bool reset(size_t size);
    bool reset(const UnsafeData& unsafeData);

#pragma mark - Subdata
public:
    Data subdata(size_t size) const;
    Data subdata(off_t offset, size_t size) const;

#pragma mark - Empty
public:
    static const Data& null();

#pragma mark - Opaque
protected:
    using UnsafeData::subdata;
    using UnsafeData::immutable;
    using UnsafeData::null;
};

} //namespace WCDB

#endif /* _WCDB_DATA_HPP */

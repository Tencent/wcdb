//
// Created by sanhuazhang on 2018/05/19
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

#include "UnsafeData.hpp"
#include <memory>

namespace WCDB {

class ShareableHighWater;
typedef std::shared_ptr<ShareableHighWater> SharedHighWater;

class WCDB_API Data final : public UnsafeData {
#pragma mark - Initialize
public:
    Data();
    Data(const UnsafeData& data);
    Data(UnsafeData&& data);
    Data(size_t size);
    Data(const unsigned char* buffer, size_t size);
    Data(const unsigned char* buffer, size_t size, SharedHighWater highWater);

    ~Data() override;

    Data& operator=(const UnsafeData& other);
    Data& operator=(UnsafeData&& other);

protected:
    using SharedData = UnsafeData::SharedData;
    using SharedBuffer = UnsafeData::SharedBuffer;
    Data(unsigned char* buffer, size_t size, const SharedBuffer& sharedBuffer);
    offset_t getCurrentOffset() const;
    size_t getSharedSize() const;

#pragma mark - Reset
public:
    bool resize(size_t size);

    bool reset(const unsigned char* buffer, size_t size, SharedHighWater highWater);
    bool reset(size_t size);
    bool reset(const UnsafeData& unsafeData);

protected:
    static void dealloc(SharedData& data);

#pragma mark - Subdata
public:
    Data subdata(size_t size) const;
    Data subdata(offset_t offset, size_t size) const;

#pragma mark - Empty
public:
    static const Data& null();

#pragma mark - Opaque
protected:
    using UnsafeData::immutable;
};

} //namespace WCDB

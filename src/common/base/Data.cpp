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

#include "Data.hpp"
#include "Assertion.hpp"
#include "HighWater.hpp"
#include "Notifier.hpp"
#include <cstring>
#include <zlib.h>

namespace WCDB {

#pragma mark - Initialize
Data::Data() : UnsafeData()
{
}

Data::Data(size_t size) : UnsafeData()
{
    reset(size);
}

Data::Data(const unsigned char* buffer, size_t size) : UnsafeData()
{
    reset(buffer, size, nullptr);
}

Data::Data(const unsigned char* buffer, size_t size, SharedHighWater highWater)
{
    reset(buffer, size, highWater);
}

Data::Data(const UnsafeData& unsafeData) : UnsafeData(unsafeData)
{
    if (!unsafeData.hasSharedBuffer()) {
        reset(unsafeData);
    }
}

Data::Data(UnsafeData&& unsafeData) : UnsafeData(std::move(unsafeData))
{
    if (!hasSharedBuffer()) {
        reset(m_buffer, m_size, nullptr);
    }
}

Data::~Data() = default;

Data& Data::operator=(const UnsafeData& unsafeData)
{
    if (unsafeData.hasSharedBuffer()) {
        UnsafeData::operator=(unsafeData);
    } else {
        reset(unsafeData);
    }
    return *this;
}

Data& Data::operator=(UnsafeData&& unsafeData)
{
    if (unsafeData.hasSharedBuffer()) {
        UnsafeData::operator=(std::move(unsafeData));
    } else {
        reset(unsafeData);
    }
    return *this;
}

offset_t Data::getCurrentOffset() const
{
    WCTAssert(m_sharedBuffer != nullptr);
    return m_buffer - m_sharedBuffer.get().buffer;
}

size_t Data::getSharedSize() const
{
    WCTAssert(m_sharedBuffer != nullptr);
    return m_sharedBuffer.get().size;
}

Data::Data(unsigned char* buffer, size_t size, const SharedBuffer& sharedBuffer)
: UnsafeData(buffer, size, sharedBuffer)
{
}

#pragma mark - Reset
bool Data::resize(size_t size)
{
    if (m_sharedBuffer != nullptr && getCurrentOffset() + size <= getSharedSize()) {
        m_size = size;
    } else {
        SharedBuffer oldSharedBuffer = m_sharedBuffer;
        unsigned char* oldBuffer = m_buffer;
        size_t oldSize = m_size;
        if (!reset(size)) {
            return false;
        }
        if (oldBuffer && oldSize > 0) {
            memcpy(m_buffer, oldBuffer, oldSize);
        }
    }
    return true;
}

bool Data::reset(const unsigned char* buffer, size_t size, SharedHighWater highWater)
{
    if (size == 0) {
        m_sharedBuffer = nullptr;
        return true;
    }
    unsigned char* newBuffer = (unsigned char*) malloc(size * sizeof(unsigned char));
    if (newBuffer == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory, Error::Level::Error));
        return false;
    }
    if (highWater != nullptr) {
        highWater->increase(size);
    }
    if (buffer != nullptr) {
        memcpy(newBuffer, buffer, size);
    }
    m_sharedBuffer = makeSharedBuffer(newBuffer, size, [highWater](SharedData& data) {
        Data::dealloc(data);
        if (highWater != nullptr) {
            highWater->decrease(data.size);
        }
    });
    m_size = size;
    m_buffer = m_sharedBuffer.get().buffer;
    return true;
}

bool Data::reset(size_t size)
{
    return reset(nullptr, size, nullptr);
}

bool Data::reset(const UnsafeData& unsafeData)
{
    return reset(unsafeData.buffer(), unsafeData.size(), nullptr);
}

void Data::dealloc(SharedData& data)
{
    free(data.buffer);
}

#pragma mark - Subdata
Data Data::subdata(size_t size) const
{
    return subdata(0, size);
}

Data Data::subdata(offset_t offset, size_t size) const
{
    if (size == 0) {
        return null();
    }
    WCTRemedialAssert(size > 0 && getCurrentOffset() + offset + size <= getSharedSize(),
                      "Memory cross-border.",
                      return null(););
    return Data(m_buffer + offset, size, m_sharedBuffer);
}

const Data& Data::null()
{
    static const Data* s_null = new Data;
    return *s_null;
}

} //namespace WCDB

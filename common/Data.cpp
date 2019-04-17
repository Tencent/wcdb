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
#include <WCDB/Data.hpp>
#include <WCDB/Notifier.hpp>
#include <zlib.h>

namespace WCDB {

#pragma mark - Initialize
Data::Data() : UnsafeData(), m_sharedBuffer(nullptr), m_sharedSize(0)
{
}

Data::Data(size_t size) : UnsafeData(), m_sharedBuffer(nullptr), m_sharedSize(0)
{
    reset(size);
}

Data::Data(const unsigned char* buffer, size_t size)
: UnsafeData(), m_sharedBuffer(nullptr), m_sharedSize(0)
{
    reset(buffer, size);
}

Data::Data(const UnsafeData& unsafeData)
{
    reset(unsafeData);
}

Data::Data(const Data& data)
: UnsafeData(data), m_sharedBuffer(data.m_sharedBuffer), m_sharedSize(data.m_sharedSize)
{
}

Data::Data(Data&& data)
: UnsafeData(std::move(data))
, m_sharedBuffer(std::move(data.m_sharedBuffer))
, m_sharedSize(data.m_sharedSize)
{
    data.m_sharedSize = 0;
}

Data& Data::operator=(const Data& other)
{
    UnsafeData::operator=(other);
    m_sharedBuffer = other.m_sharedBuffer;
    m_sharedSize = other.m_sharedSize;
    return *this;
}

Data& Data::operator=(Data&& other)
{
    UnsafeData::operator=(std::move(other));
    m_sharedBuffer = std::move(other.m_sharedBuffer);
    m_sharedSize = other.m_sharedSize;
    other.m_sharedSize = 0;
    return *this;
}

off_t Data::getCurrentOffset() const
{
    WCTInnerAssert(m_sharedBuffer != nullptr);
    return m_sharedBuffer.get() - m_buffer;
}

size_t Data::getSharedSize() const
{
    WCTInnerAssert(m_sharedBuffer != nullptr);
    return m_sharedSize;
}

Data::Data(const std::shared_ptr<unsigned char>& sharedBuffer, size_t sharedSize, off_t offset, size_t size)
: UnsafeData(sharedBuffer.get() + offset, size)
, m_sharedBuffer(sharedBuffer)
, m_sharedSize(sharedSize)
{
}

#pragma mark - Reset
bool Data::resize(size_t size)
{
    if (m_sharedBuffer != nullptr && getCurrentOffset() + size <= getSharedSize()) {
        m_size = size;
    } else {
        std::shared_ptr<unsigned char> oldSharedBuffer = m_sharedBuffer;
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

bool Data::reset(const unsigned char* buffer, size_t size)
{
    if (size == 0) {
        m_sharedBuffer = nullptr;
        m_sharedSize = 0;
        return true;
    }
    std::shared_ptr<unsigned char> newSharedBuffer(new unsigned char[size]);
    if (newSharedBuffer == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory, Error::Level::Error));
        return false;
    }
    if (buffer != nullptr) {
        memcpy(newSharedBuffer.get(), buffer, size);
    }
    m_sharedBuffer = newSharedBuffer;
    m_sharedSize = size;
    m_size = size;
    m_buffer = m_sharedBuffer.get();
    return true;
}

bool Data::reset(size_t size)
{
    return reset(nullptr, size);
}

bool Data::reset(const UnsafeData& unsafeData)
{
    return reset(unsafeData.buffer(), unsafeData.size());
}

#pragma mark - Subdata
Data Data::subdata(size_t size) const
{
    return subdata(0, size);
}

Data Data::subdata(off_t offset, size_t size) const
{
    if (size == 0) {
        return null();
    }
    WCTRemedialAssert(size > 0 && getCurrentOffset() + offset + size <= getSharedSize(),
                      "Memory cross-border.",
                      return null(););
    return Data(m_sharedBuffer, m_sharedSize, getCurrentOffset() + offset, size);
}

const Data& Data::null()
{
    static const Data* s_empty = new Data;
    return *s_empty;
}

} //namespace WCDB

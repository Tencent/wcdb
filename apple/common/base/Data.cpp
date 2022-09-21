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

#include <WCDB/Assertion.hpp>
#include <WCDB/Data.hpp>
#include <WCDB/Notifier.hpp>
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
    reset(buffer, size);
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
        reset(m_buffer, m_size);
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

off_t Data::getCurrentOffset() const
{
    WCTAssert(m_sharedBuffer != nullptr);
    return m_buffer - m_sharedBuffer->data();
}

size_t Data::getSharedSize() const
{
    WCTAssert(m_sharedBuffer != nullptr);
    return m_sharedBuffer->size();
}

Data::Data(unsigned char* buffer,
           size_t size,
           const std::shared_ptr<std::vector<unsigned char>>& sharedBuffer)
: UnsafeData(buffer, size, sharedBuffer)
{
}

#pragma mark - Reset
bool Data::resize(size_t size)
{
    if (m_sharedBuffer != nullptr && getCurrentOffset() + size <= getSharedSize()) {
        m_size = size;
    } else {
        std::shared_ptr<std::vector<unsigned char>> oldSharedBuffer = m_sharedBuffer;
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
        return true;
    }
    std::shared_ptr<std::vector<unsigned char>> newSharedBuffer
    = std::make_shared<std::vector<unsigned char>>(size);
    if (newSharedBuffer == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory, Error::Level::Error));
        return false;
    }
    if (buffer != nullptr) {
        newSharedBuffer->assign(buffer, buffer + size);
    }
    m_sharedBuffer = newSharedBuffer;
    m_size = size;
    m_buffer = m_sharedBuffer->data();
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
    return Data(m_buffer + offset, size, m_sharedBuffer);
}

const Data& Data::null()
{
    static const Data* s_null = new Data;
    return *s_null;
}

} //namespace WCDB

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
Data::Data() : m_sharedBuffer(nullptr), m_size(0), m_buffer(nullptr)
{
}

Data::Data(const std::shared_ptr<std::vector<unsigned char>> &sharedBuffer,
           unsigned char *buffer,
           size_t size)
    : m_sharedBuffer(sharedBuffer), m_buffer(buffer), m_size(size)
{
    WCTInnerAssert(
        m_sharedBuffer == nullptr || (m_buffer == nullptr && m_size == 0) ||
        m_buffer + m_size <= m_sharedBuffer->data() + m_sharedBuffer->size());
}

#pragma mark - Shared
Data::Data(size_t size) : m_sharedBuffer(new std::vector<unsigned char>(size))
{
    if (m_sharedBuffer != nullptr) {
        m_buffer = m_sharedBuffer->data();
        m_size = size;
    } else {
        setThreadedError(Error(Error::Code::NoMemory));
        m_buffer = nullptr;
        m_size = 0;
    }
}

Data::Data(const unsigned char *buffer, size_t size)
    : m_sharedBuffer(new std::vector<unsigned char>(size))
{
    if (m_sharedBuffer != nullptr) {
        if (buffer != nullptr) {
            memcpy(m_sharedBuffer->data(), buffer, size);
        }
        m_buffer = m_sharedBuffer->data();
        m_size = size;
    } else {
        setThreadedError(Error(Error::Code::NoMemory));
        m_buffer = nullptr;
        m_size = 0;
    }
}

Data Data::copy() const
{
    return Data(m_buffer, m_size);
}

bool Data::isShared() const
{
    return m_sharedBuffer != nullptr;
}

bool Data::makeShared()
{
    if (m_sharedBuffer != nullptr) {
        return true;
    }
    m_sharedBuffer.reset(new std::vector<unsigned char>(m_size));
    if (m_sharedBuffer != nullptr) {
        if (m_buffer) {
            memcpy(m_sharedBuffer->data(), m_buffer, m_size);
        }
        return true;
    }
    setThreadedError(Error(Error::Code::NoMemory));
    return false;
}

bool Data::resize(size_t size)
{
    if (isShared() && m_size >= size) {
        m_size = size;
    } else {
        auto oldSharedBuffer = m_sharedBuffer;
        m_sharedBuffer.reset(new std::vector<unsigned char>(size));
        if (m_sharedBuffer == nullptr) {
            setThreadedError(Error(Error::Code::NoMemory));
            return false;
        }
        if (m_buffer != nullptr) {
            memcpy(m_sharedBuffer->data(), m_buffer, m_size);
        }
        m_buffer = m_sharedBuffer->data();
        m_size = size;
    }
    return true;
}

#pragma mark - No Shared
Data Data::noCopyData(unsigned char *buffer, size_t size)
{
    if (buffer != nullptr && size > 0) {
        return Data(nullptr, buffer, size);
    }
    return Data::emptyData();
}

const Data Data::immutableNoCopyData(const unsigned char *buffer, size_t size)
{
    if (buffer != nullptr && size > 0) {
        return Data(nullptr, (unsigned char *) (buffer), size);
    }
    return Data::emptyData();
}

#pragma mark - Subdata
Data Data::subdata(size_t size) const
{
    return subdata(0, size);
}

Data Data::subdata(off_t offset, size_t size) const
{
    WCTRemedialAssert(m_sharedBuffer == nullptr ||
                          m_sharedBuffer->size() >= offset + size,
                      "Memory cross-border",
                      size = m_sharedBuffer->size() - (size_t) offset;);
    return Data(m_sharedBuffer, m_buffer + offset, size);
}

#pragma mark - Empty
Data Data::emptyData()
{
    return Data();
}

bool Data::empty() const
{
    return m_size == 0;
}

unsigned char *Data::emptyBuffer()
{
    static unsigned char s_empty;
    return &s_empty;
}

#pragma mark - Basic
size_t Data::size() const
{
    return m_size;
}

const unsigned char *Data::buffer() const
{
    return m_buffer ? m_buffer : emptyBuffer();
}

unsigned char *Data::buffer()
{
    return m_buffer ? m_buffer : emptyBuffer();
}

uint32_t Data::hash() const
{
    return (uint32_t) crc32(0, buffer(), (uint32_t) size());
}

} //namespace WCDB

//
// Created by sanhuazhang on 2018/07/19
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

#include "UnsafeData.hpp"
#include "Assertion.hpp"
#include <string.h>
#include <zlib.h>

namespace WCDB {

#pragma mark - Initialize
UnsafeData::UnsafeData() : m_buffer(nullptr), m_size(0), m_sharedBuffer(nullptr)
{
}

UnsafeData::UnsafeData(unsigned char *buffer, size_t size)
: m_buffer(buffer), m_size(size), m_sharedBuffer(nullptr)
{
}

UnsafeData::UnsafeData(const UnsafeData &other)
: m_buffer(other.m_buffer), m_size(other.m_size), m_sharedBuffer(other.m_sharedBuffer)
{
}

UnsafeData::UnsafeData(UnsafeData &&other)
: m_buffer(other.m_buffer)
, m_size(other.m_size)
, m_sharedBuffer(std::move(other.m_sharedBuffer))
{
    other.m_buffer = nullptr;
    other.m_size = 0;
}

UnsafeData::~UnsafeData() = default;

UnsafeData &UnsafeData::operator=(const UnsafeData &other)
{
    m_buffer = other.m_buffer;
    m_size = other.m_size;
    m_sharedBuffer = other.m_sharedBuffer;
    return *this;
}

UnsafeData &UnsafeData::operator=(UnsafeData &&other)
{
    m_buffer = other.m_buffer;
    m_size = other.m_size;
    m_sharedBuffer = std::move(other.m_sharedBuffer);
    other.m_buffer = nullptr;
    other.m_size = 0;
    return *this;
}

bool UnsafeData::operator==(const UnsafeData &other) const
{
    if (m_size != other.size()) {
        return false;
    }
    if (m_size > 0 && memcmp(m_buffer, other.m_buffer, m_size) != 0) {
        return false;
    }
    return true;
}

bool UnsafeData::operator!=(const UnsafeData &other) const
{
    return !operator==(other);
}

UnsafeData::UnsafeData(unsigned char *buffer, size_t size, const SharedBuffer &sharedBuffer)
: m_buffer(buffer), m_size(size), m_sharedBuffer(sharedBuffer)
{
}

UnsafeData::SharedBuffer
UnsafeData::makeSharedBuffer(unsigned char *buffer, size_t size, SharedBuffer::OnRecycled onRecycle)
{
    return SharedBuffer(SharedData(buffer, size), onRecycle);
}

const UnsafeData UnsafeData::immutable(const unsigned char *buffer, size_t size)
{
    return UnsafeData((unsigned char *) buffer, size);
}

#pragma mark - Subdata
UnsafeData UnsafeData::subdata(size_t size) const
{
    WCTRemedialAssert(size <= m_size, "Memory cross-border", return UnsafeData(););
    return subdata(0, size);
}

UnsafeData UnsafeData::subdata(offset_t offset, size_t size) const
{
    if (size == 0) {
        return null();
    }
    WCTRemedialAssert(
    offset + size <= m_size, "Memory cross-border", return UnsafeData(););
    return UnsafeData(m_buffer + offset, size, m_sharedBuffer);
}

#pragma mark - Empty
const UnsafeData &UnsafeData::null()
{
    static const UnsafeData *s_null = new UnsafeData;
    return *s_null;
}

bool UnsafeData::empty() const
{
    return m_size == 0;
}

unsigned char *UnsafeData::emptyBuffer()
{
    static unsigned char s_empty = '\0';
    return &s_empty;
}

#pragma mark - Basic
size_t UnsafeData::size() const
{
    return m_size;
}

uint32_t UnsafeData::hash() const
{
    // crc32 is pretty fast and have more collisions. BUT it's enough for checking whether corrupted.
    return (uint32_t) crc32(0, buffer(), (uint32_t) size());
}

const unsigned char *UnsafeData::buffer() const
{
    return m_buffer ? m_buffer : emptyBuffer();
}

unsigned char *UnsafeData::buffer()
{
    return m_buffer ? m_buffer : emptyBuffer();
}

bool UnsafeData::hasSharedBuffer() const
{
    return m_sharedBuffer != nullptr;
}

UnsafeData::SharedData::SharedData(unsigned char *buffer, size_t size)
: buffer(buffer), size(size)
{
}

UnsafeData::SharedData::SharedData(std::nullptr_t) : buffer(nullptr), size(0)
{
}

bool UnsafeData::SharedData::operator==(const std::nullptr_t &) const
{
    return buffer == nullptr;
}

bool UnsafeData::SharedData::operator!=(const std::nullptr_t &) const
{
    return buffer != nullptr;
}

} // namespace WCDB

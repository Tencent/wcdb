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
#include <WCDB/UnsafeData.hpp>
#include <zlib.h>

namespace WCDB {

#pragma mark - Initialize
UnsafeData::UnsafeData() : m_buffer(nullptr), m_size(0)
{
}

UnsafeData::UnsafeData(unsigned char *buffer, size_t size)
: m_buffer(buffer), m_size(size)
{
}

UnsafeData::UnsafeData(const UnsafeData &other)
: m_buffer(other.m_buffer), m_size(other.m_size)
{
}

UnsafeData::UnsafeData(UnsafeData &&other)
: m_buffer(other.m_buffer), m_size(other.m_size)
{
    other.m_buffer = nullptr;
    other.m_size = 0;
}

UnsafeData &UnsafeData::operator=(const UnsafeData &other)
{
    m_buffer = other.m_buffer;
    m_size = other.m_size;
    return *this;
}

UnsafeData &UnsafeData::operator=(UnsafeData &&other)
{
    m_buffer = other.m_buffer;
    m_size = other.m_size;
    other.m_buffer = nullptr;
    other.m_size = 0;
    return *this;
}

const UnsafeData UnsafeData::immutable(const unsigned char *buffer, size_t size)
{
    return UnsafeData((unsigned char *) buffer, size);
}

#pragma mark - Subdata
UnsafeData UnsafeData::subdata(size_t size) const
{
    WCTRemedialAssert(size <= m_size, "Memory cross-border", return UnsafeData(););
    return UnsafeData(m_buffer, size);
}

UnsafeData UnsafeData::subdata(off_t offset, size_t size) const
{
    WCTRemedialAssert(
    offset + size <= m_size, "Memory cross-border", return UnsafeData(););
    return UnsafeData(m_buffer + offset, size);
}

#pragma mark - Empty
const UnsafeData &UnsafeData::emptyData()
{
    static const UnsafeData *s_empty = new UnsafeData;
    return *s_empty;
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

} // namespace WCDB

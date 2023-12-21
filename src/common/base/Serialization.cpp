//
// Created by sanhuazhang on 2018/06/27
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

#include "Serialization.hpp"
#include "Assertion.hpp"
#include "FileHandle.hpp"
#include "FileManager.hpp"
#include "StringView.hpp"
#include <cstring>

namespace WCDB {

#pragma mark - SerializeIteration
SerializeIteration::SerializeIteration() : m_cursor(0)
{
}

SerializeIteration::~SerializeIteration() = default;

void SerializeIteration::seek(offset_t position)
{
    if (position < 0) {
        m_cursor = std::max((offset_t) 0, (offset_t) capacity() + position + 1);
    } else {
        m_cursor = std::min((offset_t) capacity(), position);
    }
}

void SerializeIteration::advance(offset_t offset)
{
    seek(m_cursor + offset);
}

bool SerializeIteration::isEnough(size_t size) const
{
    return size <= capacity();
}

bool SerializeIteration::canAdvance(size_t size) const
{
    return m_cursor + size <= capacity();
}

bool SerializeIteration::ended() const
{
    return m_cursor == (offset_t) capacity();
}

const unsigned char *SerializeIteration::pointee() const
{
    return data().buffer() + m_cursor;
}

const unsigned char *SerializeIteration::base() const
{
    return data().buffer();
}

size_t SerializeIteration::capacity() const
{
    return data().size();
}

#pragma mark - Serialization
Serialization::~Serialization() = default;

bool Serialization::resize(size_t size)
{
    if (size < m_data.size()) {
        return true;
    }
    return m_data.resize(std::max((size_t) size, m_data.size() * 2));
}

bool Serialization::expand(size_t expand)
{
    return resize((size_t) m_cursor + expand);
}

const UnsafeData &Serialization::data() const
{
    return m_data;
}

unsigned char *Serialization::pointee()
{
    return m_data.buffer() + m_cursor;
}

unsigned char *Serialization::base()
{
    return m_data.buffer();
}

#pragma mark - Put
bool Serialization::putSizedString(const UnsafeStringView &string)
{
    offset_t cursor = m_cursor;
    bool succeed = false;
    do {
        size_t size = string.size();
        size_t lengthOfSize = putVarint(size);
        if (lengthOfSize == 0) {
            break;
        }
        if (!expand(size)) {
            break;
        }
        memcpy(pointee(), string.data(), size);
        succeed = true;
        advance(size);
    } while (false);
    if (!succeed) {
        seek(cursor);
    }
    return true;
}

bool Serialization::putSizedData(const UnsafeData &data)
{
    offset_t cursor = m_cursor;
    bool succeed = false;
    do {
        size_t size = data.size();
        size_t lengthOfSize = putVarint(size);
        if (lengthOfSize == 0) {
            break;
        }
        if (!expand(size)) {
            break;
        }
        memcpy(pointee(), data.buffer(), size);
        succeed = true;
        advance(size);
    } while (false);
    if (!succeed) {
        seek(cursor);
    }
    return true;
}

bool Serialization::put4BytesUInt(uint32_t value)
{
    if (!expand(sizeof(uint32_t))) {
        return false;
    }
    unsigned char *p = pointee();
    p[0] = (uint8_t) (value >> 24);
    p[1] = (uint8_t) (value >> 16);
    p[2] = (uint8_t) (value >> 8);
    p[3] = (uint8_t) value;
    advance(sizeof(uint32_t));
    return true;
}

bool Serialization::put8BytesUInt(uint64_t value)
{
    offset_t cursor = m_cursor;
    if (!put4BytesUInt((uint32_t) (value >> 32)) || !put4BytesUInt((uint32_t) value)) {
        seek(cursor);
        return false;
    }
    return true;
}

bool Serialization::putString(const UnsafeStringView &string)
{
    if (!expand(string.size() + 1)) {
        return false;
    }
    unsigned char *p = pointee();
    memcpy(p, string.data(), string.size() + 1);
    return true;
}

size_t Serialization::putVarint(uint64_t value)
{
    if (!expand(9)) {
        return 0;
    }
    unsigned char *p = pointee();
    int length = 0;
    if (value <= 0x7f) {
        p[0] = value & 0x7f;
        length = 1;
    } else if (value <= 0x3fff) {
        p[0] = ((value >> 7) & 0x7f) | 0x80;
        p[1] = value & 0x7f;
        length = 2;
    } else {
        int i, j, n;
        uint8_t buf[10];
        if (value & (((uint64_t) 0xff000000) << 32)) {
            p[8] = (uint8_t) value;
            value >>= 8;
            for (i = 7; i >= 0; i--) {
                p[i] = (uint8_t) ((value & 0x7f) | 0x80);
                value >>= 7;
            }
            length = 9;
        } else {
            n = 0;
            do {
                buf[n++] = (uint8_t) ((value & 0x7f) | 0x80);
                value >>= 7;
            } while (value != 0);
            buf[0] &= 0x7f;
            WCTAssert(n <= 9);
            for (i = 0, j = n - 1; j >= 0; j--, i++) {
                p[i] = buf[j];
            }
            length = n;
        }
    }
    advance(length);
    return length;
}

Data Serialization::finalize()
{
    return m_data.subdata((size_t) m_cursor);
}

#pragma mark - Deserialization
Deserialization::Deserialization() = default;
Deserialization::~Deserialization() = default;

Deserialization::Deserialization(const UnsafeData &data)
: m_data(data), m_dataVersion(0)
{
    static_assert(slot_2_0 == ((0x7f << 14) | (0x7f)), "");
    static_assert(slot_4_2_0 == ((0xfU << 28) | (0x7f << 14) | (0x7f)), "");
}

const UnsafeData &Deserialization::data() const
{
    return m_data;
}

uint32_t Deserialization::version() const
{
    return m_dataVersion;
}

void Deserialization::setDataVersion(uint32_t version)
{
    m_dataVersion = version;
}

void Deserialization::reset(const UnsafeData &data)
{
    m_data = data;
    m_cursor = 0;
}

#pragma mark - Advance
std::pair<size_t, StringView> Deserialization::advanceSizedString()
{
    auto pair = getSizedString(m_cursor);
    if (pair.first > 0) {
        advance(pair.first);
    }
    return pair;
}

std::pair<size_t, const UnsafeData> Deserialization::advanceSizedData()
{
    auto pair = getSizedData(m_cursor);
    if (pair.first > 0) {
        advance(pair.first);
    }
    return pair;
}

std::pair<size_t, uint64_t> Deserialization::advanceVarint()
{
    auto result = getVarint(m_cursor);
    if (result.first > 0) {
        advance(result.first);
    }
    return result;
}

int64_t Deserialization::advance8BytesInt()
{
    int64_t value = get8BytesInt(m_cursor);
    advance(8);
    return value;
}

int64_t Deserialization::advance6BytesInt()
{
    int64_t value = get6BytesInt(m_cursor);
    advance(6);
    return value;
}

int32_t Deserialization::advance4BytesInt()
{
    int32_t value = get4BytesInt(m_cursor);
    advance(4);
    return value;
}

int32_t Deserialization::advance3BytesInt()
{
    int32_t value = get3BytesInt(m_cursor);
    advance(3);
    return value;
}

int32_t Deserialization::advance2BytesInt()
{
    int32_t value = get2BytesInt(m_cursor);
    advance(2);
    return value;
}

int32_t Deserialization::advance1ByteInt()
{
    int32_t value = get1ByteInt(m_cursor);
    advance(1);
    return value;
}

double Deserialization::advance8BytesDouble()
{
    double value = get8BytesDouble(m_cursor);
    advance(8);
    return value;
}

uint32_t Deserialization::advance4BytesUInt()
{
    uint32_t value = get4BytesUInt(m_cursor);
    advance(4);
    return value;
}

std::pair<size_t, StringView> Deserialization::getSizedString(offset_t offset) const
{
    size_t lengthOfSize;
    uint64_t size;
    std::tie(lengthOfSize, size) = getVarint(offset);
    if (lengthOfSize == 0 || !isEnough((size_t) offset + lengthOfSize + (size_t) size)) {
        return { 0, StringView() };
    }
    return { lengthOfSize + size, getString((size_t) offset + lengthOfSize, (size_t) size) };
}

std::pair<size_t, const UnsafeData> Deserialization::getSizedData(offset_t offset) const
{
    size_t lengthOfSize;
    uint64_t size;
    std::tie(lengthOfSize, size) = getVarint(offset);
    if (lengthOfSize == 0 || !isEnough((size_t) offset + lengthOfSize + (size_t) size)) {
        return { 0, Data::null() };
    }
    return { lengthOfSize + size, getData((size_t) offset + lengthOfSize, (size_t) size) };
}

std::pair<size_t, uint64_t> Deserialization::getVarint(offset_t offset) const
{
    if (!isEnough((size_t) offset + 1)) {
        return { 0, 0 };
    }
    const unsigned char *p = base() + offset;

    uint32_t a = *p;
    /* a: p0 (unmasked) */
    if (!(a & 0x80)) {
        return { 1, a };
    }

    if (!isEnough((size_t) offset + 2)) {
        return { 0, 0 };
    }
    ++p;

    uint32_t b = *p;
    /* b: p1 (unmasked) */
    if (!(b & 0x80)) {
        a &= 0x7f;
        a = a << 7;
        a |= b;
        return { 2, a };
    }

    /* Verify that constants are precomputed correctly */

    if (!isEnough((size_t) offset + 3)) {
        return { 0, 0 };
    }
    ++p;
    a = a << 14;
    a |= *p;
    /* a: p0<<14 | p2 (unmasked) */
    if (!(a & 0x80)) {
        a &= slot_2_0;
        b &= 0x7f;
        b = b << 7;
        a |= b;
        return { 3, a };
    }

    /* CSE1 from below */

    a &= slot_2_0;
    if (!isEnough((size_t) offset + 4)) {
        return { 0, 0 };
    }
    ++p;
    b = b << 14;
    b |= *p;
    /* b: p1<<14 | p3 (unmasked) */
    if (!(b & 0x80)) {
        b &= slot_2_0;
        /* moved CSE1 up */
        /* a &= (0x7f<<14)|(0x7f); */
        a = a << 7;
        a |= b;
        return { 4, a };
    }

    /* a: p0<<14 | p2 (masked) */
    /* b: p1<<14 | p3 (unmasked) */
    /* 1:save off p0<<21 | p1<<14 | p2<<7 | p3 (masked) */
    /* moved CSE1 up */
    /* a &= (0x7f<<14)|(0x7f); */
    b &= slot_2_0;
    uint64_t s = a;
    /* s: p0<<14 | p2 (masked) */

    if (!isEnough((size_t) offset + 5)) {
        return { 0, 0 };
    }
    ++p;
    a = a << 14;
    a |= *p;
    /* a: p0<<28 | p2<<14 | p4 (unmasked) */
    if (!(a & 0x80)) {
        /* we can skip these cause they were (effectively) done above
             ** while calculating s */
        /* a &= (0x7f<<28)|(0x7f<<14)|(0x7f); */
        /* b &= (0x7f<<14)|(0x7f); */
        b = b << 7;
        a |= b;
        s = s >> 18;
        return { 5, ((uint64_t) s) << 32 | a };
    }

    /* 2:save off p0<<21 | p1<<14 | p2<<7 | p3 (masked) */
    s = s << 7;
    s |= b;
    /* s: p0<<21 | p1<<14 | p2<<7 | p3 (masked) */

    if (!isEnough((size_t) offset + 6)) {
        return { 0, 0 };
    }
    ++p;
    b = b << 14;
    b |= *p;
    /* b: p1<<28 | p3<<14 | p5 (unmasked) */
    if (!(b & 0x80)) {
        /* we can skip this cause it was (effectively) done above in calc'ing s */
        /* b &= (0x7f<<28)|(0x7f<<14)|(0x7f); */
        a &= slot_2_0;
        a = a << 7;
        a |= b;
        s = s >> 18;
        return { 6, ((uint64_t) s) << 32 | a };
    }

    if (!isEnough((size_t) offset + 7)) {
        return { 0, 0 };
    }
    ++p;
    a = a << 14;
    a |= *p;
    /* a: p2<<28 | p4<<14 | p6 (unmasked) */
    if (!(a & 0x80)) {
        a &= slot_4_2_0;
        b &= slot_2_0;
        b = b << 7;
        a |= b;
        s = s >> 11;
        return { 7, ((uint64_t) s) << 32 | a };
    }

    /* CSE2 from below */
    a &= slot_2_0;
    if (!isEnough((size_t) offset + 8)) {
        return { 0, 0 };
    }
    ++p;
    b = b << 14;
    b |= *p;
    /* b: p3<<28 | p5<<14 | p7 (unmasked) */
    if (!(b & 0x80)) {
        b &= slot_4_2_0;
        /* moved CSE2 up */
        /* a &= (0x7f<<14)|(0x7f); */
        a = a << 7;
        a |= b;
        s = s >> 4;
        return { 8, ((uint64_t) s) << 32 | a };
    }

    if (!isEnough((size_t) offset + 9)) {
        return { 0, 0 };
    }
    ++p;
    a = a << 15;
    a |= *p;
    /* a: p4<<29 | p6<<15 | p8 (unmasked) */

    /* moved CSE2 up */
    /* a &= (0x7f<<29)|(0x7f<<15)|(0xff); */
    b &= slot_2_0;
    b = b << 8;
    a |= b;

    s = s << 4;
    b = p[-4];
    b &= 0x7f;
    b = b >> 3;
    s |= b;

    return { 9, ((uint64_t) s) << 32 | a };
}

StringView Deserialization::getString(offset_t offset, size_t size) const
{
    if (size == 0) {
        return StringView();
    }
    WCTAssert(isEnough((size_t) offset + size));
    return StringView(reinterpret_cast<const char *>(base() + offset), size);
}

const UnsafeData Deserialization::getData(offset_t offset, size_t size) const
{
    WCTAssert(isEnough((size_t) offset + size));
    return UnsafeData::immutable(base() + offset, size);
}

int64_t Deserialization::get8BytesInt(offset_t offset) const
{
    WCTAssert(isEnough((size_t) offset + 8));
    uint64_t x = get4BytesUInt(offset);
    uint32_t y = get4BytesUInt(offset + 4);
    x = (x << 32) + y;
    return *(int64_t *) &x;
}

int64_t Deserialization::get6BytesInt(offset_t offset) const
{
    WCTAssert(isEnough((size_t) offset + 6));
    return get4BytesUInt(offset + 2) + (((int64_t) 1) << 32) * get2BytesInt(offset);
}

int32_t Deserialization::get4BytesInt(offset_t offset) const
{
    WCTAssert(isEnough((size_t) offset + 4));
    const unsigned char *p = base() + offset;
    return (16777216 * (int8_t) (p[0]) | (p[1] << 16) | (p[2] << 8) | p[3]);
}

int32_t Deserialization::get3BytesInt(offset_t offset) const
{
    WCTAssert(isEnough((size_t) offset + 3));
    const unsigned char *p = base() + offset;
    return (65536 * (int8_t) (p[0]) | (p[1] << 8) | p[2]);
}

int32_t Deserialization::get2BytesInt(offset_t offset) const
{
    WCTAssert(isEnough((size_t) offset + 2));
    const unsigned char *p = base() + offset;
    return (256 * (int8_t) (p[0]) | p[1]);
}

int32_t Deserialization::get1ByteInt(offset_t offset) const
{
    WCTAssert(isEnough((size_t) offset + 1));
    return (int8_t) (base() + offset)[0];
}

double Deserialization::get8BytesDouble(offset_t offset) const
{
    WCTAssert(isEnough((size_t) offset + 8));
    uint64_t x = get4BytesUInt(offset);
    uint32_t y = get4BytesUInt(offset + 4);
    x = (x << 32) + y;
    double d;
    memcpy(&d, &x, sizeof(double));
    return d;
}

uint32_t Deserialization::get4BytesUInt(offset_t offset) const
{
    WCTAssert(isEnough((size_t) offset + 4));
    const unsigned char *p = base() + offset;
    return (((uint32_t) p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
}

#pragma mark - Serializable
Serializable::~Serializable() = default;

Data Serializable::serialize() const
{
    Serialization serialization;
    if (serialize(serialization)) {
        return serialization.finalize();
    }
    return Data::null();
}

bool Serializable::serialize(const UnsafeStringView &path) const
{
    Data data = serialize();
    if (data.empty()) {
        return false;
    }
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::OverWrite)) {
        return false;
    }
    bool succeed = fileHandle.write(data);
    fileHandle.close();
    FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(path);
    return succeed;
}

#pragma mark - Deserializable
Deserializable::~Deserializable() = default;

bool Deserializable::deserialize(const Data &data)
{
    Deserialization deserialization(data);
    return deserialize(deserialization);
}

bool Deserializable::deserialize(const UnsafeStringView &path)
{
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::ReadOnly)) {
        return false;
    }
    Data data = fileHandle.mapOrReadAllData();
    if (data.empty() || !deserialize(data)) {
        return false;
    }
    return true;
}

} //namespace WCDB

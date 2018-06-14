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
#include <WCDB/FileHandle.hpp>
#include <WCDB/Serialization.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - SerializeIteration
SerializeIteration::SerializeIteration() : m_cursor(0), m_tail(0)
{
}

SerializeIteration::SerializeIteration(const Data &data)
    : m_data(data), m_cursor(0), m_tail(data.size())
{
}

void SerializeIteration::seek(off_t position)
{
    if (position < 0) {
        m_cursor = std::min(m_tail, (off_t) m_tail + position + 1);
    } else {
        m_cursor = std::min(m_tail, position);
    }
}

void SerializeIteration::advance(off_t offset)
{
    seek(m_cursor + offset);
}

bool SerializeIteration::isEnough(size_t size) const
{
    return m_cursor + size <= m_tail;
}

bool SerializeIteration::isEnough(off_t offset, size_t size) const
{
    return offset + size <= m_tail;
}

bool SerializeIteration::ended() const
{
    return m_cursor == m_tail;
}

#pragma mark - Deserialization
static_assert(Deserialization::slot_2_0 == ((0x7f << 14) | (0x7f)), "");
static_assert(Deserialization::slot_4_2_0 ==
                  ((0xfU << 28) | (0x7f << 14) | (0x7f)),
              "");

void Deserialization::reset(const Data &data)
{
    m_data = data;
    m_cursor = 0;
    m_tail = m_data.size();
}

#pragma mark - Advance
std::string Deserialization::advanceZeroTerminatedString()
{
    std::string value = getZeroTerminatedString(m_cursor);
    advance(value.length() + 1);
    return value;
}

const unsigned char *Deserialization::advanceBLOB(size_t size)
{
    const unsigned char *blob = getBLOB(m_cursor, size);
    if (blob) {
        advance(size);
    }
    return blob;
}

const char *Deserialization::advanceCString(size_t size)
{
    const char *cstring = getCString(m_cursor, size);
    if (cstring) {
        advance(size);
    }
    return cstring;
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

std::string Deserialization::getZeroTerminatedString(off_t offset) const
{
    size_t size = 0;
    while (offset + size <= m_tail) {
        if (m_data.buffer()[offset + size] == '\0') {
            return std::string(
                reinterpret_cast<const char *>(m_data.buffer() + offset), size);
        }
        ++size;
    }
    return String::empty();
}

const unsigned char *Deserialization::getBLOB(off_t offset, size_t size)
{
    if (!isEnough(offset, size)) {
        return nullptr;
    }
    return m_data.buffer() + offset;
}

const char *Deserialization::getCString(off_t offset, size_t size)
{
    if (!isEnough(offset, size)) {
        return nullptr;
    }
    return reinterpret_cast<const char *>(m_data.buffer() + offset);
}

std::pair<size_t, uint64_t> Deserialization::getVarint(off_t offset) const
{
    if (!isEnough(offset, 1)) {
        return {0, 0};
    }
    const unsigned char *p = m_data.buffer() + offset;

    uint32_t a = *p;
    /* a: p0 (unmasked) */
    if (!(a & 0x80)) {
        return {1, a};
    }

    if (!isEnough(offset, 2)) {
        return {0, 0};
    }
    ++p;

    uint32_t b = *p;
    /* b: p1 (unmasked) */
    if (!(b & 0x80)) {
        a &= 0x7f;
        a = a << 7;
        a |= b;
        return {2, a};
    }

    /* Verify that constants are precomputed correctly */

    if (!isEnough(offset, 3)) {
        return {0, 0};
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
        return {3, a};
    }

    /* CSE1 from below */

    a &= slot_2_0;
    if (!isEnough(offset, 4)) {
        return {0, 0};
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
        return {4, a};
    }

    /* a: p0<<14 | p2 (masked) */
    /* b: p1<<14 | p3 (unmasked) */
    /* 1:save off p0<<21 | p1<<14 | p2<<7 | p3 (masked) */
    /* moved CSE1 up */
    /* a &= (0x7f<<14)|(0x7f); */
    b &= slot_2_0;
    uint64_t s = a;
    /* s: p0<<14 | p2 (masked) */

    if (!isEnough(offset, 5)) {
        return {0, 0};
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
        return {5, ((uint64_t) s) << 32 | a};
    }

    /* 2:save off p0<<21 | p1<<14 | p2<<7 | p3 (masked) */
    s = s << 7;
    s |= b;
    /* s: p0<<21 | p1<<14 | p2<<7 | p3 (masked) */

    if (!isEnough(offset, 6)) {
        return {0, 0};
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
        return {6, ((uint64_t) s) << 32 | a};
    }

    if (!isEnough(offset, 7)) {
        return {0, 0};
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
        return {7, ((uint64_t) s) << 32 | a};
    }

    /* CSE2 from below */
    a &= slot_2_0;
    if (!isEnough(offset, 8)) {
        return {0, 0};
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
        return {8, ((uint64_t) s) << 32 | a};
    }

    if (!isEnough(offset, 9)) {
        return {0, 0};
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

    return {9, ((uint64_t) s) << 32 | a};
}

int64_t Deserialization::get8BytesInt(off_t offset) const
{
    if (!isEnough(offset, 8)) {
        return 0;
    }
    uint64_t x = get4BytesUInt(offset);
    uint32_t y = get4BytesUInt(offset + 4);
    x = (x << 32) + y;
    return *(int64_t *) &x;
}

int64_t Deserialization::get6BytesInt(off_t offset) const
{
    if (!isEnough(offset, 6)) {
        return 0;
    }
    return get4BytesInt(offset + 2) +
           (((int64_t) 1) << 32) * get2BytesInt(offset);
}

int32_t Deserialization::get4BytesInt(off_t offset) const
{
    if (!isEnough(offset, 4)) {
        return 0;
    }
    const unsigned char *buffer = m_data.buffer();
    return (16777216 * (int8_t)(buffer[0]) | (buffer[1] << 16) |
            (buffer[2] << 8) | buffer[3]);
}

int32_t Deserialization::get3BytesInt(off_t offset) const
{
    if (!isEnough(offset, 3)) {
        return 0;
    }
    const unsigned char *buffer = m_data.buffer();
    return (65536 * (int8_t)(buffer[0]) | (buffer[1] << 8) | buffer[2]);
}

int32_t Deserialization::get2BytesInt(off_t offset) const
{
    if (!isEnough(offset, 2)) {
        return 0;
    }
    const unsigned char *buffer = m_data.buffer();
    return (256 * (int8_t)(buffer[0]) | buffer[1]);
}

int32_t Deserialization::get1ByteInt(off_t offset) const
{
    if (!isEnough(offset, 1)) {
        return 0;
    }
    const unsigned char *buffer = m_data.buffer();
    return (int8_t) buffer[0];
}

double Deserialization::get8BytesDouble(off_t offset) const
{
    if (!isEnough(offset, 8)) {
        return 0;
    }
    uint64_t x = get4BytesUInt(offset);
    uint32_t y = get4BytesUInt(offset + 4);
    x = (x << 32) + y;
    double d;
    memcpy(&d, &x, sizeof(double));
    return d;
}

uint32_t Deserialization::get4BytesUInt(off_t offset) const
{
    if (!isEnough(offset, 4)) {
        return 0;
    }
    const unsigned char *buffer = m_data.buffer();
    return (((uint32_t) buffer[0] << 24) | (buffer[1] << 16) |
            (buffer[2] << 8) | buffer[3]);
}

const Data &Deserialization::getData() const
{
    return m_data;
}

#pragma mark - Serialization
Serialization::Serialization() : SerializeIteration()
{
}

bool Serialization::resizeToFit(size_t size)
{
    if (m_cursor + size < m_data.size()) {
        return m_data.resize(
            std::max((size_t) m_cursor + size, m_data.size() * 2));
    }
    return true;
}

unsigned char *Serialization::pointee()
{
    return m_data.buffer() + m_cursor;
}

#pragma mark - Put
bool Serialization::putZeroTerminatedString(const std::string &value)
{
    size_t size = value.length() + 1;
    if (!resizeToFit(size)) {
        return false;
    }
    memcpy(pointee(), value.data(), size);
    expand(m_cursor + size);
    advance(size);
    return true;
}

bool Serialization::putBLOB(const Data &data)
{
    if (!resizeToFit(data.size())) {
        return false;
    }
    memcpy(pointee(), data.buffer(), data.size());
    expand(m_cursor + data.size());
    advance(data.size());
    return true;
}

bool Serialization::put4BytesUInt(uint32_t value)
{
    if (!resizeToFit(4)) {
        return false;
    }
    unsigned char *p = pointee();
    p[0] = (uint8_t)(value >> 24);
    p[1] = (uint8_t)(value >> 16);
    p[2] = (uint8_t)(value >> 8);
    p[3] = (uint8_t) value;
    expand(m_cursor + sizeof(uint32_t));
    advance(sizeof(uint32_t));
    return true;
}

size_t Serialization::putVarint(uint64_t value)
{
    if (!resizeToFit(9)) {
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
                p[i] = (uint8_t)((value & 0x7f) | 0x80);
                value >>= 7;
            }
            length = 9;
        } else {
            n = 0;
            do {
                buf[n++] = (uint8_t)((value & 0x7f) | 0x80);
                value >>= 7;
            } while (value != 0);
            buf[0] &= 0x7f;
            WCTInnerAssert(n <= 9);
            for (i = 0, j = n - 1; j >= 0; j--, i++) {
                p[i] = buf[j];
            }
            length = n;
        }
    }
    expand(m_cursor + length);
    advance(length);
    return length;
}

void Serialization::expand(off_t newTail)
{
    m_tail = std::max(m_tail, newTail);
}

Data Serialization::finalize()
{
    return m_data.subdata((size_t) m_cursor);
}

#pragma mark - Deserializable
bool Deserializable::deserialize(const Data &data)
{
    Deserialization deserialization(data);
    return deserialize(deserialization);
}

bool Deserializable::deserialize(const std::string &path)
{
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::ReadOnly)) {
        return false;
    }
    bool succeed = false;
    do {
        ssize_t size = fileHandle.size();
        if (size < 0) {
            break;
        }
        Data data(size);
        ssize_t read = fileHandle.read(data.buffer(), 0, size);
        if (read != size) {
            if (read >= 0) {
                Error error;
                error.setCode(Error::Code::IOError);
                error.message = "Short read.";
                error.infos.set("Path", path);
                Notifier::shared()->notify(error);
                setThreadedError(std::move(error));
            }
            break;
        }
        succeed = deserialize(data);
    } while (false);
    fileHandle.close();
    return succeed;
}

#pragma mark - Serializable
Data Serializable::serialize() const
{
    Serialization serialization;
    if (serialize(serialization)) {
        return serialization.finalize();
    }
    return Data::emptyData();
}

bool Serializable::serialize(const std::string &path) const
{
    Data data = serialize();
    if (data.empty()) {
        return false;
    }
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::OverWrite)) {
        return false;
    }
    bool succeed = false;
    do {
        ssize_t wrote = fileHandle.write(data.buffer(), 0, data.size());
        if (wrote != data.size()) {
            if (wrote >= 0) {
                Error error;
                error.setCode(Error::Code::IOError);
                error.message = "Short write.";
                error.infos.set("Path", path);
                Notifier::shared()->notify(error);
                setThreadedError(std::move(error));
            }
            break;
        }
        succeed = true;
    } while (false);
    fileHandle.close();
    return succeed;
}

} //namespace Repair

} //namespace WCDB

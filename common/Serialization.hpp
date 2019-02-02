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

#ifndef __WCDB_SERIALIZATION_HPP
#define __WCDB_SERIALIZATION_HPP

#include <WCDB/Data.hpp>

namespace WCDB {

class SerializeIteration {
#pragma mark - SerializeIteration
public:
    SerializeIteration();
    virtual ~SerializeIteration();

    void seek(off_t position);
    void advance(off_t offset);
    bool canAdvance(size_t size) const;
    bool isEnough(size_t size) const;
    bool ended() const;

protected:
    virtual const UnsafeData &data() const = 0;
    const unsigned char *pointee() const;
    const unsigned char *base() const;
    size_t capacity() const;

    off_t m_cursor;
};

class Serialization final : public SerializeIteration {
#pragma mark - Serialization
public:
    using SerializeIteration::SerializeIteration;

    Data finalize();
    bool resize(size_t size);
    bool expand(size_t expand);

protected:
    unsigned char *pointee();
    unsigned char *base();
    const UnsafeData &data() const override final;
    Data m_data;

#pragma mark - Put
public:
    bool putSizedString(const String &string);
    bool putSizedData(const UnsafeData &data);
    bool put4BytesUInt(uint32_t value);
    bool put8BytesUInt(uint64_t value);
    bool putString(const String &string);
    size_t putVarint(uint64_t value);
};

class Deserialization final : public SerializeIteration {
#pragma mark - Deserialization
public:
    using SerializeIteration::SerializeIteration;
    Deserialization(const UnsafeData &data);

    void reset(const UnsafeData &data);

    const UnsafeData &data() const override final;

protected:
    UnsafeData m_data;

    static constexpr const int slot_2_0 = 0x001fc07f;
    static constexpr const int slot_4_2_0 = 0xf01fc07f;

#pragma mark - Advance
public:
    //return 0 size to indicate failure
    std::pair<size_t, String> advanceSizedString();
    std::pair<size_t, const UnsafeData> advanceSizedData();
    std::pair<size_t, uint64_t> advanceVarint();

    // For the following types with specified size, `canAdvance` should be called first.
    int64_t advance8BytesInt();
    int64_t advance6BytesInt();
    int32_t advance4BytesInt();
    int32_t advance3BytesInt();
    int32_t advance2BytesInt();
    int32_t advance1ByteInt();
    double advance8BytesDouble();
    uint32_t advance4BytesUInt();

#pragma mark - Get
public:
    //return 0 size to indicate failure
    std::pair<size_t, String> getSizedString(off_t offset) const;
    std::pair<size_t, const UnsafeData> getSizedData(off_t offset) const;
    std::pair<size_t, uint64_t> getVarint(off_t offset) const;

    // For the following types with specified size, `isEnough` should be called first.
    String getString(off_t offset, size_t size) const;
    const UnsafeData getData(off_t offset, size_t size) const;
    int64_t get8BytesInt(off_t offset) const;
    int64_t get6BytesInt(off_t offset) const;
    int32_t get4BytesInt(off_t offset) const;
    int32_t get3BytesInt(off_t offset) const;
    int32_t get2BytesInt(off_t offset) const;
    int32_t get1ByteInt(off_t offset) const;
    double get8BytesDouble(off_t offset) const;
    uint32_t get4BytesUInt(off_t offset) const;
};

#pragma mark - Serializable
class Serializable : protected SharedThreadedErrorProne {
public:
    virtual ~Serializable();
    Data serialize() const;
    bool serialize(const String &path) const;
    virtual bool serialize(Serialization &serialization) const = 0;
};

#pragma mark - Deserializable
class Deserializable : protected SharedThreadedErrorProne {
public:
    virtual ~Deserializable();
    bool deserialize(const Data &data);
    bool deserialize(const String &path);
    virtual bool deserialize(Deserialization &deserialization) = 0;
};

} //namespace WCDB

#endif /* __WCDB_SERIALIZATION_HPP */

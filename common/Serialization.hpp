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

#ifndef Serialization_hpp
#define Serialization_hpp

#include <WCDB/Data.hpp>

namespace WCDB {

class SerializeIteration {
#pragma mark - SerializeIteration
public:
    SerializeIteration();
    SerializeIteration(const Data &data);

    void seek(off_t position);
    void advance(off_t offset);
    bool canAdvance(size_t size) const;
    bool isEnough(size_t size) const;
    bool ended() const;

protected:
    unsigned char *pointee();
    const unsigned char *pointee() const;
    unsigned char *base();
    const unsigned char *base() const;
    size_t capacity() const;

    off_t m_cursor;
    Data m_data;
};

class Serialization : public SerializeIteration {
#pragma mark - Serialization
public:
    Serialization();

    Data finalize();
    bool resize(size_t size);
    bool expand(size_t expand);

#pragma mark - Put
public:
    bool putSizedString(const std::string &string);
    bool putSizedData(const Data &data);
    bool put4BytesUInt(uint32_t value);
    bool put8BytesUInt(uint64_t value);
    bool putString(const std::string &string);
    size_t putVarint(uint64_t value);
};

class Deserialization : public SerializeIteration {
#pragma mark - Deserialization
public:
    using SerializeIteration::SerializeIteration;
    void reset(const Data &data);

    const Data &getData() const;

    static constexpr const int slot_2_0 = 0x001fc07f;
    static constexpr const int slot_4_2_0 = 0xf01fc07f;

#pragma mark - Advance
public:
    //return 0 size to indicate failure
    std::pair<size_t, std::string> advanceSizedString();
    std::pair<size_t, const Data> advanceSizedData();
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
    std::pair<size_t, std::string> getSizedString(off_t offset) const;
    std::pair<size_t, const Data> getSizedData(off_t offset) const;
    std::pair<size_t, uint64_t> getVarint(off_t offset) const;

    // For the following types with specified size, `isEnough` should be called first.
    std::string getString(off_t offset, size_t size) const;
    const Data getData(off_t offset, size_t size) const;
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
    Data serialize() const;
    bool serialize(const std::string &path) const;
    virtual bool serialize(Serialization &serialization) const = 0;
};

#pragma mark - Deserializable
class Deserializable : protected SharedThreadedErrorProne {
public:
    bool deserialize(const Data &data);
    bool deserialize(const std::string &path);
    virtual bool deserialize(Deserialization &deserialization) = 0;
};

} //namespace WCDB

#endif /* Serialization_hpp */

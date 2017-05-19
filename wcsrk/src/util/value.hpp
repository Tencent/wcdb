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

#ifndef value_hpp
#define value_hpp

#include <memory>
#include <vector>

enum class ValueType
{
    Integer,
    Float,
    Text,
    BLOB,
    Null,
};

class BaseValue
{
public:
    const ValueType type;
protected:
    BaseValue(ValueType theType);
};

class IntegerValue : public BaseValue
{
public:
    IntegerValue(int64_t theValue);
    
    const int64_t value;
};

class FloatValue : public BaseValue
{
public:
    FloatValue(double theValue);
    
    const double value;
};

class TextValue : public BaseValue
{
public:
    TextValue(const char* theValue);
    TextValue(const char* theValue, const size_t& theSize);

    const char* value;
    const size_t size;
};

class BLOBValue : public BaseValue
{
public:
    BLOBValue(const unsigned char* theBytes, const size_t& theSize);
    
    const unsigned char* bytes;
    const size_t size;
};

class NullValue : public BaseValue
{
public:
    NullValue();
};

class Value
{
public:
    template <typename T>
    Value(const T& value, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr)
    : m_value(new IntegerValue(value)) {
    }

    template <typename T>
    Value(const T& value, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr)
    : m_value(new FloatValue(value)) {
    }

    Value(const char* value);

    Value(const char* value, const size_t& size);
    
    Value(const unsigned char* bytes, const size_t& size);

    Value(std::nullptr_t = nullptr);

    int64_t getIntegerValue() const;

    double getFloatValue() const;

    //it may return a non-termintor string
    const char* getTextValue() const;

    const unsigned char* getBLOBValue() const;

    size_t getSize() const;

    ValueType getType() const;
protected:
    const std::shared_ptr<BaseValue> m_value;
};

typedef std::vector<Value> Values;

#endif /* value_hpp */

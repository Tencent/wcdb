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

#include "value.hpp"

BaseValue::BaseValue(ValueType theType)
: type(theType)
{
}

IntegerValue::IntegerValue(int64_t theValue)
: BaseValue(ValueType::Integer)
, value(theValue)
{
}

FloatValue::FloatValue(double theValue)
: BaseValue(ValueType::Float)
, value(theValue) 
{
}

TextValue::TextValue(const char* theValue)
: BaseValue(ValueType::Text)
, value(theValue)
, size(strlen(theValue))
{
}

TextValue::TextValue(const char* theValue, const size_t& theSize)
: BaseValue(ValueType::Text)
, value(theValue)
, size(theSize)
{
}

BLOBValue::BLOBValue(const unsigned char* theBytes, const size_t& theSize)
: BaseValue(ValueType::BLOB)
, bytes(theBytes)
, size(theSize) 
{
}

NullValue::NullValue()
: BaseValue(ValueType::Null)
{
}

Value::Value(const char* value)
: m_value(new TextValue(value)) 
{
}


Value::Value(const char* value, const size_t& size)
: m_value(new TextValue(value, size))
{
}

Value::Value(const unsigned char* bytes, const size_t& size)
: m_value(new BLOBValue(bytes, size))
{
}

Value::Value(std::nullptr_t)
: m_value(new NullValue())
{
}

int64_t Value::getIntegerValue() const 
{
    return m_value->type==ValueType::Integer?static_cast<IntegerValue*>(m_value.get())->value:0;
}

double Value::getFloatValue() const 
{
    return m_value->type==ValueType::Float?static_cast<FloatValue*>(m_value.get())->value:0;
}

const char* Value::getTextValue() const 
{
    return m_value->type==ValueType::Text?static_cast<TextValue*>(m_value.get())->value:nullptr;
}

const unsigned char* Value::getBLOBValue() const 
{
    return m_value->type==ValueType::BLOB?static_cast<BLOBValue*>(m_value.get())->bytes:nullptr;
}

size_t Value::getSize() const 
{
    if (m_value->type==ValueType::BLOB) {
        return static_cast<BLOBValue*>(m_value.get())->size;
    }else if (m_value->type==ValueType::Text) {
        return static_cast<TextValue*>(m_value.get())->size;
    }
    return 0;
}

ValueType Value::getType() const 
{
    return m_value->type;
}

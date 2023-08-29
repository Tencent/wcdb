//
// Created by qiuwenchen on 2022/8/10.
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

#include "Value.hpp"
#include <cstring>
#include <math.h>

namespace WCDB {

Value::Value() : m_type(Type::Null)
{
    m_intValue = 0;
}

Value::Value(const Value &other)
{
    m_type = other.m_type;
    copyValue(other);
}

Value::Value(Value &&other)
{
    m_type = other.m_type;
    moveValue(other);
}

Value::~Value()
{
    clearValue();
}

Value &Value::operator=(const Value &other)
{
    clearValue();
    m_type = other.m_type;
    copyValue(other);
    return *this;
}

Value &Value::operator=(Value &&other)
{
    clearValue();
    m_type = other.m_type;
    moveValue(other);
    return *this;
}

bool Value::operator==(const Value &other) const
{
    if (m_type != other.m_type) {
        return false;
    }
    switch (m_type) {
    case Type::Null:
        return true;
    case Type::Integer:
        return m_intValue == other.m_intValue;
    case Type::Float:
        return m_floatValue == other.m_floatValue;
    case Type::Text:
        return (m_textValue).compare(other.m_textValue) == 0;
    case Type::BLOB: {
        if (m_blobValue.size() != other.m_blobValue.size()) {
            return false;
        }
        return memcmp(
               m_blobValue.buffer(), other.m_blobValue.buffer(), m_blobValue.size())
               == 0;
    }
    }
}

bool Value::operator!=(const Value &other) const
{
    return !(operator==(other));
}

Value::operator bool() const
{
    return boolValue();
}

Value::operator StringView() const
{
    return textValue();
}

Value::operator std::string() const
{
    StringView text = textValue();
    return std::string(text.data(), text.size());
}

Value::operator Data() const
{
    return blobValue();
}

void Value::clearValue()
{
    switch (m_type) {
    case Type::Integer: {
        m_intValue = 0;
    } break;
    case Type::Float: {
        m_floatValue = 0;
    } break;
    case Type::Text: {
        m_textValue.~StringView();
    } break;
    case Type::BLOB: {
        m_blobValue.~Data();
    } break;
    case Type::Null: {
        return;
    }
    }
}

void Value::copyValue(const Value &other)
{
    switch (other.m_type) {
    case Type::Integer: {
        m_intValue = other.m_intValue;
    } break;
    case Type::Float: {
        m_floatValue = other.m_floatValue;
    } break;
    case Type::Text: {
        new ((void *) std::addressof(m_textValue)) StringView(other.m_textValue);
    } break;
    case Type::BLOB: {
        new ((void *) std::addressof(m_blobValue)) Data(other.m_blobValue);
    } break;
    case Type::Null: {
        return;
    }
    }
}

void Value::moveValue(Value &other)
{
    switch (other.m_type) {
    case Type::Integer: {
        m_intValue = other.m_intValue;
    } break;
    case Type::Float: {
        m_floatValue = other.m_floatValue;
    } break;
    case Type::Text: {
        new ((void *) std::addressof(m_textValue))
        StringView(std::move(other.m_textValue));
    } break;
    case Type::BLOB: {
        new ((void *) std::addressof(m_blobValue)) Data(std::move(other.m_blobValue));
    } break;
    case Type::Null: {
        return;
    }
    }
}

Value::Type Value::getType() const
{
    return m_type;
}

bool Value::boolValue() const
{
    return intValue() > 0;
}

int64_t Value::intValue() const
{
    switch (m_type) {
    case Type::Integer:
        return m_intValue;
    case Type::Float:
        return (int64_t) m_floatValue;
    case Type::Text: {
        if (m_textValue.size() > 0) {
            return atoll(m_textValue.data());
        } else {
            return 0;
        }
    }
    case Type::BLOB: {
        if (m_blobValue.size() > 0) {
            return atoll((const char *) m_blobValue.buffer());
        } else {
            return 0;
        }
    }
    case Type::Null: {
        return 0;
    }
    }
}
double Value::floatValue() const
{
    switch (m_type) {
    case Type::Integer:
        return m_intValue;
    case Type::Float:
        return m_floatValue;
    case Type::Text: {
        if (m_textValue.size() > 0) {
            double num = atof(m_textValue.data());
            if (isnan(num) || isinf(num)) return 0;
            return num;
        } else {
            return 0;
        }
    }
    case Type::BLOB: {
        if (m_blobValue.size() > 0) {
            double num = atof((const char *) m_blobValue.buffer());
            if (isnan(num) || isinf(num)) return 0;
            return num;
        } else {
            return 0;
        }
    }
    case Type::Null: {
        return 0;
    }
    }
}

StringView Value::textValue() const
{
    switch (m_type) {
    case Type::Integer:
        return StringView::formatted("%lld", m_intValue);
    case Type::Float:
        return StringView::formatted("%f", m_floatValue);
    case Type::Text: {
        return m_textValue;
    }
    case Type::BLOB: {
        return StringView((const char *) m_blobValue.buffer(), m_blobValue.size());
    }
    case Type::Null: {
        return StringView();
    }
    }
}

Data Value::blobValue() const
{
    switch (m_type) {
    case Type::Integer: {
        StringView string = StringView::formatted("%lld", m_intValue);
        return Data((const unsigned char *) string.data(), string.length());
    }
    case Type::Float: {
        StringView string = StringView::formatted("%f", m_floatValue);
        return Data((const unsigned char *) string.data(), string.length());
    }
    case Type::Text: {
        return Data((const unsigned char *) m_textValue.data(), m_textValue.length());
    }
    case Type::BLOB: {
        return m_blobValue;
    }
    case Type::Null: {
        return Data();
    }
    }
}

bool Value::isNull() const
{
    return m_type == Type::Null;
}

bool Value::isEmpty() const
{
    switch (m_type) {
    case Type::Integer:
        return m_intValue == 0;
    case Type::Float:
        return m_floatValue == 0;
    case Type::Text:
        return m_textValue.length() == 0;
    case Type::BLOB:
        return m_blobValue.size() == 0;
    case Type::Null:
        return true;
    }
}

} // namespace WCDB

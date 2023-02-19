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

#include <WCDB/Value.hpp>
#include <cstring>
#include <math.h>

namespace WCDB {

Value::Value() : m_type(Type::Null)
{
    m_value.intValue = 0;
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
        return m_value.intValue == other.m_value.intValue;
    case Type::Float:
        return m_value.floatValue == other.m_value.floatValue;
    case Type::Text:
        return (*m_value.textValue).compare(*other.m_value.textValue) == 0;
    case Type::BLOB: {
        if (m_value.blobValue->size() != other.m_value.blobValue->size()) {
            return false;
        }
        return memcmp(m_value.blobValue->buffer(),
                      other.m_value.blobValue->buffer(),
                      m_value.blobValue->size())
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
        m_value.intValue = 0;
    } break;
    case Type::Float: {
        m_value.floatValue = 0;
    } break;
    case Type::Text: {
        if (m_value.textValue) {
            delete m_value.textValue;
            m_value.textValue = nullptr;
        }
    } break;
    case Type::BLOB: {
        if (m_value.blobValue) {
            delete m_value.blobValue;
            m_value.blobValue = nullptr;
        }
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
        m_value.intValue = other.m_value.intValue;
    } break;
    case Type::Float: {
        m_value.floatValue = other.m_value.floatValue;
    } break;
    case Type::Text: {
        if (other.m_value.textValue) {
            m_value.textValue = new StringView(*other.m_value.textValue);
        }
    } break;
    case Type::BLOB: {
        if (other.m_value.blobValue) {
            m_value.blobValue = new Data(*other.m_value.blobValue);
        }
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
        m_value.intValue = other.m_value.intValue;
    } break;
    case Type::Float: {
        m_value.floatValue = other.m_value.floatValue;
    } break;
    case Type::Text: {
        m_value.textValue = other.m_value.textValue;
        other.m_value.textValue = nullptr;
    } break;
    case Type::BLOB: {
        m_value.blobValue = other.m_value.blobValue;
        other.m_value.blobValue = nullptr;
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
        return m_value.intValue;
    case Type::Float:
        return (int64_t) m_value.floatValue;
    case Type::Text: {
        if (m_value.textValue != nullptr && m_value.textValue->size() > 0) {
            return atoll(m_value.textValue->data());
        } else {
            return 0;
        }
    }
    case Type::BLOB: {
        if (m_value.blobValue != nullptr && m_value.blobValue->size() > 0) {
            return atoll((const char *) m_value.blobValue->buffer());
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
        return m_value.intValue;
    case Type::Float:
        return m_value.floatValue;
    case Type::Text: {
        if (m_value.textValue != nullptr && m_value.textValue->size() > 0) {
            double num = atof(m_value.textValue->data());
            if (isnan(num) || isinf(num)) return 0;
            return num;
        } else {
            return 0;
        }
    }
    case Type::BLOB: {
        if (m_value.blobValue != nullptr && m_value.blobValue->size() > 0) {
            double num = atof((const char *) m_value.blobValue->buffer());
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
        return StringView::formatted("%lld", m_value.intValue);
    case Type::Float:
        return StringView::formatted("%f", m_value.floatValue);
    case Type::Text: {
        if (m_value.textValue == nullptr || m_value.textValue->length() == 0) {
            return StringView();
        }
        return *m_value.textValue;
    }
    case Type::BLOB: {
        if (m_value.blobValue == nullptr || m_value.blobValue->size() == 0) {
            return StringView();
        }
        return StringView((const char *) m_value.blobValue->buffer(),
                          m_value.blobValue->size());
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
        StringView string = StringView::formatted("%lld", m_value.intValue);
        return Data((const unsigned char *) string.data(), string.length());
    }
    case Type::Float: {
        StringView string = StringView::formatted("%f", m_value.floatValue);
        return Data((const unsigned char *) string.data(), string.length());
    }
    case Type::Text: {
        if (m_value.textValue == nullptr || m_value.textValue->size() == 0) {
            return Data();
        }
        return Data((const unsigned char *) m_value.textValue->data(),
                    m_value.textValue->length());
    }
    case Type::BLOB: {
        if (m_value.blobValue == nullptr || m_value.blobValue->size() == 0) {
            return Data();
        }
        return *m_value.blobValue;
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
        return m_value.intValue == 0;
    case Type::Float:
        return m_value.floatValue == 0;
    case Type::Text:
        return m_value.textValue->length() == 0;
    case Type::BLOB:
        return m_value.blobValue->size() == 0;
    case Type::Null:
        return true;
    }
}

} // namespace WCDB

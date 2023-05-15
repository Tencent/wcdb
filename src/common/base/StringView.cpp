//
// Created by sanhuazhang on 2019/06/16
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

#include "StringView.hpp"
#include "Macro.h"
#include "UnsafeData.hpp"
#include <cstring>
#include <iomanip>
#include <numeric>
#include <zlib.h>

namespace WCDB {

#pragma mark - UnsafeStringView - Constructor
UnsafeStringView::UnsafeStringView() = default;

UnsafeStringView::UnsafeStringView(const char* string)
: m_data(string != nullptr ? string : "")
, m_length(string != nullptr ? (int) strlen(string) : 0)
, m_referenceCount(nullptr)
{
}

UnsafeStringView::UnsafeStringView(const char* string, size_t length)
: m_data(string != nullptr ? string : ""), m_length(length), m_referenceCount(nullptr)
{
}

UnsafeStringView::UnsafeStringView(const UnsafeStringView& other)
: m_data(other.m_data), m_length(other.m_length), m_referenceCount(other.m_referenceCount)
{
    if ((uint64_t) m_referenceCount > ConstanceReference) {
        (*m_referenceCount)++;
    }
}

UnsafeStringView::UnsafeStringView(UnsafeStringView&& other)
: m_data(other.m_data), m_length(other.m_length), m_referenceCount(other.m_referenceCount)
{
    other.m_referenceCount = nullptr;
}

UnsafeStringView::~UnsafeStringView()
{
    if ((uint64_t) m_referenceCount <= ConstanceReference) {
        return;
    }
    (*m_referenceCount)--;
    tryClearSpace();
};

UnsafeStringView& UnsafeStringView::operator=(const UnsafeStringView& other)
{
    m_data = other.m_data;
    m_length = other.m_length;
    m_referenceCount = other.m_referenceCount;
    if (((uint64_t) m_referenceCount > ConstanceReference)) {
        (*m_referenceCount)++;
    }
    return *this;
}

UnsafeStringView& UnsafeStringView::operator=(UnsafeStringView&& other)
{
    m_data = other.m_data;
    m_length = other.m_length;
    m_referenceCount = other.m_referenceCount;
    other.m_referenceCount = nullptr;
    return *this;
}

bool UnsafeStringView::operator==(const UnsafeStringView& other) const
{
    return compare(other) == 0;
}

bool UnsafeStringView::operator!=(const UnsafeStringView& other) const
{
    return compare(other) != 0;
}

bool UnsafeStringView::operator<(const UnsafeStringView& other) const
{
    return compare(other) < 0;
}

bool UnsafeStringView::operator>(const UnsafeStringView& other) const
{
    return compare(other) > 0;
}

const char& UnsafeStringView::operator[](off_t off) const
{
    return m_data[off];
}

UnsafeStringView::operator std::string() const
{
    if (m_length > 0) {
        return std::string(m_data, m_length);
    }
    return std::string();
}

const char* UnsafeStringView::data() const
{
    return m_data;
}

size_t UnsafeStringView::length() const
{
    return m_length;
}

size_t UnsafeStringView::size() const
{
    return m_length;
}

bool UnsafeStringView::empty() const
{
    return m_length == 0;
}

const char& UnsafeStringView::at(off_t off) const
{
    return m_data[off];
}

#pragma mark - UnsafeStringView - Comparison
int UnsafeStringView::caseInsensiveCompare(const UnsafeStringView& other) const
{
    if (m_data == other.m_data && m_length == other.m_length) {
        return 0;
    }
    int ret = strncasecmp(m_data, other.m_data, std::min(m_length, other.m_length));
    if (ret != 0) {
        return ret;
    }
    if (m_length > other.m_length) {
        return 1;
    } else if (m_length < other.m_length) {
        return -1;
    } else {
        return 0;
    }
}

bool UnsafeStringView::caseInsensiveEqual(const UnsafeStringView& other) const
{
    if (length() != other.length()) {
        return false;
    }
    return strncasecmp(data(), other.data(), m_length) == 0;
}

#pragma mark - UnsafeStringView - Operations
uint32_t UnsafeStringView::hash() const
{
    return (uint32_t) crc32(0, (const unsigned char*) data(), (uint32_t) m_length);
}

bool UnsafeStringView::hasPrefix(const UnsafeStringView& target) const
{
    bool has = false;
    if (length() >= target.length()) {
        has = strncmp(data(), target.data(), target.m_length) == 0;
    }
    return has;
}

bool UnsafeStringView::hasSuffix(const UnsafeStringView& target) const
{
    bool has = false;
    if (m_length >= target.m_length) {
        has = strncmp(data() + m_length - target.m_length, target.data(), target.m_length) == 0;
    }
    return has;
}

bool UnsafeStringView::contain(const UnsafeStringView& target) const
{
    if (target.m_length > m_length || target.m_length == 0) {
        return false;
    }
    return strstr(m_data, target.m_data) != nullptr;
}

int UnsafeStringView::compare(const UnsafeStringView& other) const
{
    if (m_data == other.m_data && m_length == other.m_length) {
        return 0;
    }
    int ret = strncmp(m_data, other.m_data, std::min(m_length, other.m_length));
    if (ret != 0) {
        return ret;
    }
    if (m_length > other.m_length) {
        return 1;
    } else if (m_length < other.m_length) {
        return -1;
    } else {
        return 0;
    }
}

size_t UnsafeStringView::find(const UnsafeStringView& other) const
{
    if (other.m_length > m_length || other.m_length == 0) {
        return npos;
    }
    const char* ret = strstr(m_data, other.m_data);
    if (ret == nullptr) {
        return npos;
    }
    return ret - m_data;
}

#pragma mark - UnsafeStringView - Memory Management
void UnsafeStringView::clear()
{
    ensureNewSpace(0);
    m_data = "";
}

void UnsafeStringView::ensureNewSpace(size_t newSize)
{
    if ((uint64_t) m_referenceCount <= ConstanceReference) {
        if (newSize > 0) {
            createNewSpace(newSize);
        } else {
            m_referenceCount = nullptr;
        }
    } else {
        (*m_referenceCount)--;
        if (newSize > m_length || newSize == 0) {
            tryClearSpace();
            if (newSize > 0) {
                createNewSpace(newSize);
            } else {
                m_referenceCount = nullptr;
            }
        } else {
            if (*m_referenceCount > 0) {
                createNewSpace(newSize);
            } else {
                *m_referenceCount = 1;
            }
        }
    }
    m_length = newSize;
}

void UnsafeStringView::createNewSpace(size_t newSize)
{
    constexpr int referenceSize = sizeof(std::atomic<int>);
    m_referenceCount = (std::atomic<int>*) malloc(referenceSize + newSize + 1);
    if (m_referenceCount != nullptr) {
        new (m_referenceCount) std::atomic<int>(1);
    }
}

void UnsafeStringView::tryClearSpace()
{
    if (*m_referenceCount == 0) {
        m_referenceCount->~atomic<int>();
        free(m_referenceCount);
    }
}

#pragma mark - StringView - Constructor
StringView::StringView() = default;

void StringView::assignString(const char* content, size_t length)
{
    if (length == 0 && content != nullptr) {
        length = strlen(content);
    }
    ensureNewSpace(length);
    if (content == nullptr || length == 0 || m_referenceCount == nullptr) {
        m_data = "";
        return;
    }
    char* data = (char*) (m_referenceCount + 1);
    memcpy(data, (void*) content, length);
    data[length] = '\0';
    m_data = data;
}

StringView::StringView(const char* string) : UnsafeStringView()
{
    assignString(string, 0);
}

StringView::StringView(const char* string, size_t length) : UnsafeStringView()
{
    assignString(string, length);
}

StringView::StringView(std::string&& string) : UnsafeStringView()
{
    assignString(string.data(), string.length());
}

StringView::StringView(const UnsafeStringView& other)
: UnsafeStringView(other.m_referenceCount != nullptr ? other : UnsafeStringView())
{
    if (other.m_referenceCount == nullptr) {
        assignString(other.m_data, other.m_length);
    }
}

StringView::StringView(UnsafeStringView&& other)
: UnsafeStringView(other.m_referenceCount != nullptr ? std::move(other) : UnsafeStringView())
{
    if (other.m_referenceCount == nullptr) {
        assignString(other.m_data, other.m_length);
    }
}

StringView& StringView::operator=(const UnsafeStringView& other)
{
    if (other.m_referenceCount != nullptr) {
        UnsafeStringView::operator=(other);
    } else {
        assignString(other.m_data, other.m_length);
    }
    return *this;
}

StringView& StringView::operator=(UnsafeStringView&& other)
{
    if (other.m_referenceCount != nullptr) {
        UnsafeStringView::operator=(std::move(other));
    } else {
        assignString(other.m_data, other.m_length);
    }
    return *this;
}

#pragma mark - StringView - Utility
StringView StringView::formatted(const char* format, ...)
{
    size_t size = strlen(format) * 2 + 50;
    std::string result;
    va_list ap;
    do {
        result.resize(size);
        va_start(ap, format);
        int n = vsnprintf((char*) result.data(), size, format, ap);
        va_end(ap);
        if (n > -1 && n < (int) size) {
            result.resize(n);
            return StringView(std::move(result));
        }
        if (n > -1) {
            size = n + 1;
        } else {
            size *= 2;
        }
    } while (true);
    return StringView(std::move(result));
}

StringView StringView::hexString(const UnsafeData& data)
{
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < data.size(); i++) {
        int c = data.buffer()[i];
        ss << std::setw(2) << c;
    }
    return StringView(ss.str());
}

StringView StringView::makeConstant(const char* string)
{
    StringView ret;
    if (string != nullptr) {
        ret.m_data = string;
        ret.m_length = strlen(string);
        ret.m_referenceCount = (std::atomic<int>*) ConstanceReference;
    }
    return ret;
}

StringView StringView::createConstant(const char* string)
{
    StringView ret;
    if (string != nullptr) {
        size_t length = strlen(string);
        char* data = (char*) malloc((length + 1) * sizeof(char));
        if (data != nullptr) {
            memcpy((void*) data, (void*) string, length);
            data[length] = '\0';
        } else {
            length = 0;
        }
        ret.m_data = data;
        ret.m_length = length;
        ret.m_referenceCount = (std::atomic<int>*) ConstanceReference;
    }
    return ret;
}

bool StringViewComparator::operator()(const StringView& lhs, const StringView& rhs) const
{
    return lhs.compare(rhs) < 0;
}

bool StringViewComparator::operator()(const StringView& lhs, const UnsafeStringView& rhs) const
{
    return lhs.compare(rhs) < 0;
}

bool StringViewComparator::operator()(const UnsafeStringView& lhs, const StringView& rhs) const
{
    return lhs.compare(rhs) < 0;
}

void StringViewSet::erase(const UnsafeStringView& value)
{
    auto iter = this->find(value);
    if (iter != this->end()) {
        this->Super::erase(iter);
    }
}

} // namespace WCDB

std::ostream& operator<<(std::ostream& stream, const WCDB::UnsafeStringView& string)
{
    stream.write(string.data(), string.length());
    return stream;
}

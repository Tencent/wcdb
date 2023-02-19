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

#include <WCDB/Macro.h>
#include <WCDB/StringView.hpp>
#include <WCDB/UnsafeData.hpp>
#include <cstring>
#include <iomanip>
#include <numeric>
#include <zlib.h>

namespace WCDB {

#pragma mark - UnsafeStringView - Constructor
UnsafeStringView::UnsafeStringView()
: m_data(""), m_length(0), m_buffer(nullptr)
{
}

UnsafeStringView::UnsafeStringView(const char* string)
: m_data(string != nullptr ? string : "")
, m_length(string != nullptr ? strlen(string) : 0)
, m_buffer(nullptr)
{
}

UnsafeStringView::UnsafeStringView(const char* string, size_t length)
: m_data(string != nullptr ? string : ""), m_length(length), m_buffer(nullptr)
{
}

UnsafeStringView::UnsafeStringView(const UnsafeStringView& other)
: m_data(other.m_data), m_length(other.m_length), m_buffer(other.m_buffer)
{
}

UnsafeStringView::UnsafeStringView(UnsafeStringView&& other)
: m_data(other.m_data), m_length(other.m_length), m_buffer(std::move(other.m_buffer))
{
}

UnsafeStringView::UnsafeStringView(std::shared_ptr<const std::string>&& buffer)
: m_data(buffer != nullptr ? buffer->data() : "")
, m_length(buffer != nullptr ? buffer->length() : 0)
, m_buffer(std::move(buffer))
{
}

UnsafeStringView::~UnsafeStringView() = default;

UnsafeStringView& UnsafeStringView::operator=(const UnsafeStringView& other)
{
    m_data = other.m_data;
    m_length = other.m_length;
    m_buffer = other.m_buffer;
    return *this;
}

UnsafeStringView& UnsafeStringView::operator=(UnsafeStringView&& other)
{
    m_data = other.m_data;
    m_length = other.m_length;
    m_buffer = std::move(other.m_buffer);
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

void UnsafeStringView::assign(std::shared_ptr<const std::string>&& buffer)
{
    if (buffer != nullptr) {
        m_data = buffer->data();
        m_length = buffer->length();
        m_buffer = std::move(buffer);
    } else {
        m_data = nullptr;
        m_length = 0;
        m_buffer = nullptr;
    }
}

#pragma mark - UnsafeStringView - Comparison
int UnsafeStringView::caseInsensiveCompare(const UnsafeStringView& other) const
{
    return strcasecmp(data(), other.data());
}

bool UnsafeStringView::caseInsensiveEqual(const UnsafeStringView& other) const
{
    if (length() != other.length()) {
        return false;
    }
    return strcasecmp(data(), other.data()) == 0;
}

#pragma mark - UnsafeStringView - Operations
uint32_t UnsafeStringView::hash() const
{
    return (uint32_t) crc32(0, (const unsigned char*) data(), (uint32_t) length());
}

bool UnsafeStringView::hasPrefix(const UnsafeStringView& target) const
{
    bool has = false;
    if (length() >= target.length()) {
        has = strncmp(data(), target.data(), target.length()) == 0;
    }
    return has;
}

bool UnsafeStringView::hasSuffix(const UnsafeStringView& target) const
{
    bool has = false;
    if (length() >= target.length()) {
        has = strncmp(data() + length() - target.length(), target.data(), target.length()) == 0;
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

#pragma mark - UnsafeStringView - Modifier
void UnsafeStringView::clear()
{
    assign(nullptr);
}

#pragma mark - StringView - Constructor
StringView::StringView() = default;
StringView::~StringView() = default;

StringView::StringView(const char* string)
: UnsafeStringView(string != nullptr ? std::make_shared<const std::string>(string) : nullptr)
{
}

StringView::StringView(const char* string, size_t length)
: UnsafeStringView(string != nullptr ? std::make_shared<const std::string>(string, length) : nullptr)
{
}

StringView::StringView(std::string&& string)
: UnsafeStringView(std::make_shared<const std::string>(std::move(string)))
{
}

StringView::StringView(const UnsafeStringView& other)
: UnsafeStringView(
other.m_buffer != nullptr ?
std::shared_ptr<const std::string>(other.m_buffer) :
(other.empty() ? nullptr : std::make_shared<std::string>(other.data(), other.length())))
{
}

StringView::StringView(UnsafeStringView&& other)
: UnsafeStringView(
other.m_buffer != nullptr ?
std::move(other.m_buffer) :
(other.empty() ? nullptr : std::make_shared<std::string>(other.data(), other.length())))
{
}

StringView& StringView::operator=(const UnsafeStringView& other)
{
    if (other.m_buffer != nullptr) {
        UnsafeStringView::operator=(other);
    } else if (!other.empty()) {
        assign(std::make_shared<const std::string>(other.data(), other.length()));
    } else {
        assign(nullptr);
    }
    return *this;
}

StringView& StringView::operator=(UnsafeStringView&& other)
{
    if (other.m_buffer != nullptr) {
        UnsafeStringView::operator=(std::move(other));
    } else if (!other.empty()) {
        assign(std::make_shared<const std::string>(other.data(), other.length()));
    } else {
        assign(nullptr);
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

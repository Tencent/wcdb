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

#include <WCDB/Macro.hpp>
#include <WCDB/StringView.hpp>
#include <zlib.h>

namespace WCDB {

#pragma mark - UnsafeStringView - Constructor
UnsafeStringView::UnsafeStringView()
: std::string_view(emptyView()), m_buffer(nullptr)
{
}

UnsafeStringView::UnsafeStringView(const char* string)
: std::string_view(string != nullptr ? std::string_view(string) : emptyView())
{
}

UnsafeStringView::UnsafeStringView(const char* string, size_t length)
: std::string_view(string != nullptr && length > 0 ? std::string_view(string, length) :
                                                     emptyView())
{
}

UnsafeStringView::UnsafeStringView(const UnsafeStringView& other)
: std::string_view(other), m_buffer(other.m_buffer)
{
}

UnsafeStringView::UnsafeStringView(UnsafeStringView&& other)
: std::string_view(std::move(other)), m_buffer(std::move(other.m_buffer))
{
}

UnsafeStringView::UnsafeStringView(std::shared_ptr<const std::string>&& buffer)
: std::string_view(
buffer != nullptr ? std::string_view(buffer->data(), buffer->length()) : emptyView())
, m_buffer(std::move(buffer))
{
}

UnsafeStringView::~UnsafeStringView() = default;

UnsafeStringView& UnsafeStringView::operator=(const UnsafeStringView& other)
{
    std::string_view::operator=(other);
    m_buffer = other.m_buffer;
    return *this;
}

UnsafeStringView& UnsafeStringView::operator=(UnsafeStringView&& other)
{
    std::string_view::operator=(std::move(other));
    m_buffer = std::move(other.m_buffer);
    return *this;
}

void UnsafeStringView::assign(std::shared_ptr<const std::string>&& buffer)
{
    if (buffer != nullptr) {
        std::string_view::operator=(std::string_view(buffer->data(), buffer->size()));
        m_buffer = std::move(buffer);
    } else {
        std::string_view::operator=(emptyView());
        m_buffer = nullptr;
    }
}

const std::string_view& UnsafeStringView::emptyView()
{
    WCDB_STATIC_VARIABLE const std::string_view s_emptyView("", 0);
    return s_emptyView;
}

//#pragma mark - UnsafeStringView - Ownership
//bool UnsafeStringView::owned() const
//{
//    return m_buffer != nullptr || empty();
//}

#pragma mark - UnsafeStringView - Comparison
int UnsafeStringView::caseInsensiveCompare(const UnsafeStringView& other) const
{
    return strcasecmp(data(), other.data());
}

bool UnsafeStringView::caseInsensiveEqual(const UnsafeStringView& other) const
{
    return caseInsensiveCompare(other) == 0;
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
        if (n > -1 && n < size) {
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

bool StringViewComparator::operator()(const StringView& lhs, const StringView& rhs) const
{
    return lhs < rhs;
}

bool StringViewComparator::
operator()(const StringView& lhs, const UnsafeStringView& rhs) const
{
    return lhs < rhs;
}

bool StringViewComparator::
operator()(const UnsafeStringView& lhs, const StringView& rhs) const
{
    return lhs < rhs;
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

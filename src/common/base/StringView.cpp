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
#include "Assertion.hpp"
#include "CrossPlatform.h"
#include "Macro.h"
#include "UnsafeData.hpp"
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <numeric>
#include <zlib.h>

namespace WCDB {

constexpr int kReferenceSize = sizeof(std::atomic<int>) / sizeof(char);

#pragma mark - UnsafeStringView - Constructor
UnsafeStringView::UnsafeStringView()
: m_data(""), m_length(0), m_referenceCount(nullptr)
{
}

UnsafeStringView::UnsafeStringView(const char* string)
: m_data(string != nullptr ? string : "")
, m_length(string != nullptr ? (int) strlen(string) : 0)
, m_referenceCount(nullptr)
{
#ifdef __ANDROID__
    if (tryRetrievePreAllocatedMemory(string)) {
        m_referenceCount = (std::atomic<int>*) (string - kReferenceSize);
        new (m_referenceCount) std::atomic<int>(1);
    }
#endif
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
    other.m_data = "";
    other.m_length = 0;
}

UnsafeStringView::~UnsafeStringView()
{
    if ((uint64_t) m_referenceCount <= ConstanceReference) {
        return;
    }
    if (--(*m_referenceCount) == 0) {
        clearSpace();
    }
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
    other.m_data = "";
    other.m_length = 0;
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

const char& UnsafeStringView::operator[](offset_t off) const
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

const char& UnsafeStringView::at(offset_t off) const
{
    return m_data[off];
}

UnsafeStringView UnsafeStringView::subStr(offset_t off, size_t length) const
{
    WCTRemedialAssert(off + length <= m_length, "invalid parameter", UnsafeStringView(););
    if (length == 0) {
        length = m_length - off;
    }
    UnsafeStringView ret(m_data + off, length);
    ret.m_referenceCount = m_referenceCount;
    if (((uint64_t) m_referenceCount > ConstanceReference)) {
        (*m_referenceCount)++;
    }
    return ret;
}

#pragma mark - UTF16

#ifdef _WIN32
std::wstring UnsafeStringView::getWString() const
{
    std::wstring wstring;
    int wlen = MultiByteToWideChar(CP_UTF8, 0, m_data, m_length, NULL, 0);
    if (wlen > 0) {
        wstring.resize(static_cast<size_t>(wlen));
        MultiByteToWideChar(CP_UTF8, 0, m_data, m_length, &wstring[0], wlen);
    }
    return wstring;
}
#endif

#define READ_UTF16LE(zIn, TERM, c)                                                      \
    {                                                                                   \
        c = (*zIn++);                                                                   \
        c += ((*zIn++) << 8);                                                           \
        if (c >= 0xD800 && c < 0xE000 && TERM) {                                        \
            int c2 = (*zIn++);                                                          \
            c2 += ((*zIn++) << 8);                                                      \
            c = (c2 & 0x03FF) + ((c & 0x003F) << 10) + (((c & 0x03C0) + 0x0040) << 10); \
        }                                                                               \
    }

#define READ_UTF16BE(zIn, TERM, c)                                                      \
    {                                                                                   \
        c = ((*zIn++) << 8);                                                            \
        c += (*zIn++);                                                                  \
        if (c >= 0xD800 && c < 0xE000 && TERM) {                                        \
            int c2 = ((*zIn++) << 8);                                                   \
            c2 += (*zIn++);                                                             \
            c = (c2 & 0x03FF) + ((c & 0x003F) << 10) + (((c & 0x03C0) + 0x0040) << 10); \
        }                                                                               \
    }

#define u8 unsigned char

#define WRITE_UTF8(zOut, c)                                                    \
    {                                                                          \
        if (c < 0x00080) {                                                     \
            *zOut++ = (u8) (c & 0xFF);                                         \
        } else if (c < 0x00800) {                                              \
            *zOut++ = 0xC0 + (u8) ((c >> 6) & 0x1F);                           \
            *zOut++ = 0x80 + (u8) (c & 0x3F);                                  \
        } else if (c < 0x10000) {                                              \
            *zOut++ = 0xE0 + (u8) ((c >> 12) & 0x0F);                          \
            *zOut++ = 0x80 + (u8) ((c >> 6) & 0x3F);                           \
            *zOut++ = 0x80 + (u8) (c & 0x3F);                                  \
        } else {                                                               \
            *zOut++ = 0xF0 + (u8) ((c >> 18) & 0x07);                          \
            *zOut++ = 0x80 + (u8) ((c >> 12) & 0x3F);                          \
            *zOut++ = 0x80 + (u8) ((c >> 6) & 0x3F);                           \
            *zOut++ = 0x80 + (u8) (c & 0x3F);                                  \
        }                                                                      \
    }

UnsafeStringView
UnsafeStringView::createFromUTF16(const char16_t* utf16Str, size_t length, char* buffer)
{
    return UnsafeStringView(buffer, changeToUTF8(utf16Str, length, buffer));
}

size_t UnsafeStringView::changeToUTF8(const char16_t* utf16Str, size_t length, char* buffer)
{
    if (length == 0) {
        return length;
    }
    const char* in = (const char*) utf16Str;
    const char* end = (const char*) (utf16Str + length);
    char* out = buffer;
    unsigned int c;
    static bool s_isBigEndian = []() -> bool {
        short int n = 0x1;
        char* p = (char*) &n;
        return p[0] != 1;
    }();
    while (in < end) {
        if (s_isBigEndian) {
            READ_UTF16BE(in, in < end, c)
            WRITE_UTF8(out, c);
        } else {
            READ_UTF16LE(in, in < end, c)
            WRITE_UTF8(out, c);
        }
    }
    *out = 0;
    return out - buffer;
}

#pragma mark - UnsafeStringView - Comparison
int UnsafeStringView::caseInsensitiveCompare(const UnsafeStringView& other) const
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

bool UnsafeStringView::caseInsensitiveEqual(const UnsafeStringView& other) const
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

bool UnsafeStringView::equal(const UnsafeStringView& other) const
{
    return compare(other) == 0;
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

size_t UnsafeStringView::find(const UnsafeStringView& other, off_t off) const
{
    if (off + other.m_length > m_length || other.m_length == 0 || off < 0) {
        return npos;
    }
    const char* ret = strstr(m_data + off, other.m_data);
    if (ret == nullptr) {
        return npos;
    }
    size_t pos = ret - m_data;
    if (pos >= m_length) {
        return npos;
    }
    return pos;
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
        int count = --(*m_referenceCount);
        if (newSize > m_length || newSize == 0) {
            if (count == 0) {
                clearSpace();
            }
            if (newSize > 0) {
                createNewSpace(newSize);
            } else {
                m_referenceCount = nullptr;
            }
        } else {
            if (count > 0) {
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
    m_referenceCount = (std::atomic<int>*) malloc(kReferenceSize + newSize + 1);
    if (m_referenceCount != nullptr) {
        new (m_referenceCount) std::atomic<int>(1);
    }
}

void UnsafeStringView::clearSpace()
{
    m_referenceCount->~atomic<int>();
    free(m_referenceCount);
    m_referenceCount = nullptr;
}

#ifdef __ANDROID__

#pragma mark - UnsafeStringView - PreAllocMemory

thread_local UnsafeStringView::PreAllocatedMemory UnsafeStringView::g_preAllocatedMemory
= { nullptr, 0, 0 };

char** UnsafeStringView::preAllocStringMemorySlot(int count)
{
    if (count == 0) {
        return nullptr;
    }
    if (g_preAllocatedMemory.totalCount - g_preAllocatedMemory.usedCount < count) {
        int neededCount = g_preAllocatedMemory.usedCount + count;
        int newCount = 1;
        while (newCount < neededCount) {
            newCount = newCount << 1;
        }
        char** newMemory
        = (char**) realloc(g_preAllocatedMemory.memory, newCount * sizeof(char*));
        if (newMemory == nullptr) {
            return nullptr;
        }
        memset(newMemory + g_preAllocatedMemory.totalCount,
               0,
               sizeof(char*) * (newCount - g_preAllocatedMemory.totalCount));
        g_preAllocatedMemory.memory = newMemory;
        g_preAllocatedMemory.totalCount = newCount;
    }
    char** ret = g_preAllocatedMemory.memory + g_preAllocatedMemory.usedCount;
    g_preAllocatedMemory.usedCount += count;
    return ret;
}

void UnsafeStringView::allocStringMemory(char** slot, int size)
{
    if (size == 0 || slot == nullptr) {
        return;
    }
    char* buffer = (char*) malloc((size + 1 + kReferenceSize) * sizeof(char));
    if (buffer == nullptr) {
        return;
    }
    *slot = buffer + kReferenceSize;
}

void UnsafeStringView::clearAllocatedMemory(int count)
{
    char** memory = g_preAllocatedMemory.memory;
    for (int i = g_preAllocatedMemory.usedCount - count;
         i < g_preAllocatedMemory.usedCount;
         i++) {
        if (memory[i] == nullptr) {
            continue;
        }
        free(memory[i] - kReferenceSize);
        memory[i] = nullptr;
    }
    g_preAllocatedMemory.usedCount -= count;
}

void UnsafeStringView::clearAllPreAllocatedMemory()
{
    WCTAssert(g_preAllocatedMemory.usedCount == 0);
    clearAllocatedMemory(g_preAllocatedMemory.usedCount);
}

bool UnsafeStringView::tryRetrievePreAllocatedMemory(const char* string)
{
    if (string == nullptr) {
        return false;
    }
    for (int i = 0; i < g_preAllocatedMemory.usedCount; i++) {
        if (g_preAllocatedMemory.memory[i] != string) {
            continue;
        }
        g_preAllocatedMemory.memory[i] = nullptr;
        if (g_preAllocatedMemory.usedCount == i + 1) {
            int j = i - 1;
            for (; j >= 0 && g_preAllocatedMemory.memory[j] == nullptr; j--)
                ;
            g_preAllocatedMemory.usedCount = j + 1;
        }
        WCTAssert(g_preAllocatedMemory.usedCount >= 0);
        return true;
    }
    return false;
}

#endif

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

#pragma mark - UTF16
StringView StringView::createFromUTF16(const char16_t* utf16Str, size_t length)
{
    StringView ret;
    if (length == 0 || utf16Str == nullptr) {
        return ret;
    }
    ret.ensureNewSpace(4 * length + 1);
    ret.m_data = (char*) (ret.m_referenceCount + 1);
    ret.m_length = changeToUTF8(utf16Str, length, const_cast<char*>(ret.m_data));
    return ret;
}

StringView::StringView(const char* string) : UnsafeStringView()
{
#ifdef __ANDROID__
    if (tryRetrievePreAllocatedMemory(string)) {
        m_referenceCount = (std::atomic<int>*) (string - kReferenceSize);
        new (m_referenceCount) std::atomic<int>(1);
        m_data = string;
        m_length = strlen(string);
        return;
    }
#endif
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
    if (m_referenceCount == nullptr) {
        assignString(other.m_data, other.m_length);
    }
}

StringView::StringView(UnsafeStringView&& other)
: UnsafeStringView(other.m_referenceCount != nullptr ? std::move(other) : UnsafeStringView())
{
    if (m_referenceCount == nullptr) {
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

StringView StringView::createConstant(const char* string, size_t length)
{
    StringView ret;
    if (string != nullptr) {
        if (length == 0) {
            length = strlen(string);
        }
#ifdef __ANDROID__
        if (tryRetrievePreAllocatedMemory(string)) {
            ret.m_data = string;
            ret.m_length = length;
            ret.m_referenceCount = (std::atomic<int>*) ConstanceReference;
            return ret;
        }
#endif
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
#ifdef _WIN32
StringView StringView::createFromWString(const wchar_t* string)
{
    int length = WideCharToMultiByte(CP_UTF8, 0, string, -1, NULL, 0, NULL, NULL);
    if (length <= 1) {
        return StringView();
    }
    char buffer[_MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, string, -1, buffer, length, NULL, NULL);
    return StringView(buffer, length - 1);
}
#endif

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

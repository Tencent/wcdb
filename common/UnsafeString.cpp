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

#include <WCDB/UnsafeString.hpp>
#include <zlib.h>

namespace WCDB {

UnsafeString::UnsafeString() : m_cstring(nullptr)
{
}

UnsafeString::UnsafeString(const char* str) : m_cstring(str)
{
}

const char* UnsafeString::cstring() const
{
    return m_cstring ? m_cstring : emptyCString();
}

UnsafeString::operator const char*() const
{
    return cstring();
}

const UnsafeString& UnsafeString::null()
{
    static const UnsafeString s_null(emptyCString());
    return s_null;
}

const char* UnsafeString::emptyCString()
{
    static const char s_emptyCString = '\0';
    return &s_emptyCString;
}

} //namespace WCDB

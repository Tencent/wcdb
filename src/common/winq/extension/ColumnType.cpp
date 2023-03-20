//
// Created by sanhuazhang on 2019/05/02
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

#include "ColumnType.hpp"
#include <cstring>

namespace WCDB {

#pragma mark - Builtin Type
//NULL
ColumnTypeInfo<ColumnType::Null>::UnderlyingType
ColumnIsNullType<std::nullptr_t>::asUnderlyingType(const std::nullptr_t &)
{
    return nullptr;
}

//Text
ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<const char *>::asUnderlyingType(const char *text)
{
    return UnsafeStringView(text);
}

const char *ColumnIsTextType<const char *>::fromUnderlyingType(
const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
{
    int64_t length = t.length() + 1;
    char *text = new char[length];
    memcpy(text, t.data(), length * sizeof(char));
    return text;
};

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<char *>::asUnderlyingType(const char *text)
{
    return UnsafeStringView(text);
}

char *
ColumnIsTextType<char *>::fromUnderlyingType(const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
{
    int64_t length = t.length() + 1;
    char *text = new char[length];
    memcpy(text, t.data(), length * sizeof(char));
    return text;
};

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<std::string>::asUnderlyingType(const std::string &text)
{
    return UnsafeStringView(text.c_str(), text.length());
}

std::string ColumnIsTextType<std::string>::fromUnderlyingType(
const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
{
    return std::string(t.data());
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<UnsafeStringView>::asUnderlyingType(const UnsafeStringView &text)
{
    return text;
}

UnsafeStringView ColumnIsTextType<UnsafeStringView>::fromUnderlyingType(
const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
{
    return StringView(t);
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<StringView>::asUnderlyingType(const UnsafeStringView &text)
{
    return text;
}

StringView ColumnIsTextType<StringView>::fromUnderlyingType(
const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
{
    return StringView(t);
}

//BLOB
ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<std::vector<unsigned char>>::asUnderlyingType(const std::vector<unsigned char> &blob)
{
    return UnsafeData::immutable(blob.data(), blob.size());
}

std::vector<unsigned char> ColumnIsBLOBType<std::vector<unsigned char>>::fromUnderlyingType(
const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t)
{
    std::vector<unsigned char> data;
    data.assign(t.buffer(), t.buffer() + t.size());
    return data;
}

ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<UnsafeData>::asUnderlyingType(const UnsafeData &blob)
{
    return blob;
}

UnsafeData ColumnIsBLOBType<UnsafeData>::fromUnderlyingType(
const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t)
{
    return Data(t);
}

ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<Data>::asUnderlyingType(const UnsafeData &blob)
{
    return blob;
}

Data ColumnIsBLOBType<Data>::fromUnderlyingType(const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t)
{
    return t;
}

} //namespace WCDB

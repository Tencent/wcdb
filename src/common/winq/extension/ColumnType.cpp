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
//Text
ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<const char *>::asUnderlyingType(const char *text)
{
    return UnsafeStringView(text);
}

void ColumnIsTextType<const char *>::setToUnderlyingType(
const char *&target, const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
{
    int64_t length = t.length() + 1;
    char *text = new char[length];
    memcpy(text, t.data(), length * sizeof(char));
    target = text;
};

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<char *>::asUnderlyingType(const char *text)
{
    return UnsafeStringView(text);
}

void ColumnIsTextType<char *>::setToUnderlyingType(
char *&target, const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
{
    int64_t length = t.length() + 1;
    char *text = new char[length];
    memcpy(text, t.data(), length * sizeof(char));
    target = text;
};

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<std::string>::asUnderlyingType(const std::string &text)
{
    return UnsafeStringView(text.c_str(), text.length());
}

void ColumnIsTextType<std::string>::setToUnderlyingType(
std::string &target, const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
{
    target = t;
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<UnsafeStringView>::asUnderlyingType(const UnsafeStringView &text)
{
    return text;
}

void ColumnIsTextType<UnsafeStringView>::setToUnderlyingType(
UnsafeStringView &target, const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
{
    target = StringView(t);
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<StringView>::asUnderlyingType(const UnsafeStringView &text)
{
    return text;
}

void ColumnIsTextType<StringView>::setToUnderlyingType(
StringView &target, const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
{
    target = t;
}

//BLOB
ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<std::vector<unsigned char>>::asUnderlyingType(const std::vector<unsigned char> &blob)
{
    return UnsafeData::immutable(blob.data(), blob.size());
}

void ColumnIsBLOBType<std::vector<unsigned char>>::setToUnderlyingType(
std::vector<unsigned char> &target, const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t)
{
    target.resize(t.size());
    memcpy(target.data(), t.buffer(), t.size());
}

ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<UnsafeData>::asUnderlyingType(const UnsafeData &blob)
{
    return blob;
}

void ColumnIsBLOBType<UnsafeData>::setToUnderlyingType(
UnsafeData &target, const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t)
{
    target = Data(t);
}

ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<Data>::asUnderlyingType(const UnsafeData &blob)
{
    return blob;
}

void ColumnIsBLOBType<Data>::setToUnderlyingType(
Data &target, const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t)
{
    target = t;
}

} //namespace WCDB

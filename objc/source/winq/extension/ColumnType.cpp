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

#include <WCDB/ColumnType.hpp>

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
    return UnsafeString(text, -1);
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<char *>::asUnderlyingType(const char *text)
{
    return UnsafeString(text, -1);
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<std::string>::asUnderlyingType(const std::string &text)
{
    return UnsafeString(text.c_str(), text.length());
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<UnsafeString>::asUnderlyingType(const UnsafeString &text)
{
    return text;
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<String>::asUnderlyingType(const String &text)
{
    return text;
}

//BLOB
ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<std::vector<unsigned char>>::asUnderlyingType(const std::vector<unsigned char> &blob)
{
    return UnsafeData::immutable(blob.data(), blob.size());
}

ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<UnsafeData>::asUnderlyingType(const UnsafeData &blob)
{
    return blob;
}

} //namespace WCDB

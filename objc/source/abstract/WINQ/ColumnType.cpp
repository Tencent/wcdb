//
//  ColumnType.cpp
//  WCDB
//
//  Created by sanhuazhang on 2018/4/18.
//  Copyright © 2018 sanhuazhang. All rights reserved.
//

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
    return text;
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<char *>::asUnderlyingType(const char *text)
{
    return text;
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType
ColumnIsTextType<std::string>::asUnderlyingType(const std::string &text)
{
    return text.c_str();
}

//BLOB
ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<std::vector<unsigned char>>::asUnderlyingType(
    const std::vector<unsigned char> &blob)
{
    return NoCopyData(blob.data(), blob.size());
}

ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
ColumnIsBLOBType<NoCopyData>::asUnderlyingType(const NoCopyData &blob)
{
    return blob;
}

} //namespace WCDB

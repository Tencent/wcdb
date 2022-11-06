//
// Created by 陈秋文 on 2022/10/30.
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

#include "CPPFTS5Object.h"

CPPFTS5Object::CPPFTS5Object() = default;

CPPFTS5Object::CPPFTS5Object(WCDB::UnsafeStringView cont, WCDB::UnsafeStringView ext)
: content(cont)
, extension(ext)
{
}

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPFTS5Object)
WCDB_CPP_SYNTHESIZE(content)
WCDB_CPP_SYNTHESIZE(extension)

WCDB_CPP_VIRTUAL_TABLE_MODULE(WCDB::ModuleFTS5)
WCDB_CPP_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS(WCDB::TokenizerVerbatim, WCDB::TokenizerParameter_SimplifyChinese)
WCDB_CPP_ORM_IMPLEMENTATION_END

bool CPPFTS5Object::operator==(const CPPFTS5Object &other)
{
    return content.compare(other.content) == 0 && extension.compare(other.extension) == 0;
}

CPPFTS5PinyinObject::CPPFTS5PinyinObject() = default;

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPFTS5PinyinObject)

WCDB_CPP_SYNTHESIZE(content)
WCDB_CPP_VIRTUAL_TABLE_MODULE(WCDB::ModuleFTS5)
WCDB_CPP_VIRTUAL_TABLE_TOKENIZE(WCDB::TokenizerPinyin)

WCDB_CPP_ORM_IMPLEMENTATION_END

bool CPPFTS5PinyinObject::operator==(const CPPFTS5PinyinObject &other)
{
    return content.compare(other.content) == 0;
}

CPPFTS5SymbolObject::CPPFTS5SymbolObject() = default;

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPFTS5SymbolObject)

WCDB_CPP_SYNTHESIZE(content)
WCDB_CPP_VIRTUAL_TABLE_MODULE(WCDB::ModuleFTS5)
WCDB_CPP_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS(WCDB::TokenizerVerbatim, WCDB::TokenizerParameter_NeedSymbol)

WCDB_CPP_ORM_IMPLEMENTATION_END

bool CPPFTS5SymbolObject::operator==(const CPPFTS5SymbolObject &other)
{
    return content.compare(other.content) == 0;
}

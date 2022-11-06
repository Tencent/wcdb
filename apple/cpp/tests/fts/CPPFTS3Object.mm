//
// Created by 陈秋文 on 2022/10/29.
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

#include "CPPFTS3Object.h"

CPPFTS3Object::CPPFTS3Object() = default;

CPPFTS3Object::CPPFTS3Object(WCDB::UnsafeStringView cont, WCDB::UnsafeStringView ext)
: content(cont)
, extension(ext)
{
}

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPFTS3Object)
WCDB_CPP_SYNTHESIZE(content)
WCDB_CPP_SYNTHESIZE(extension)

WCDB_CPP_VIRTUAL_TABLE_MODULE(WCDB::ModuleFTS3)
WCDB_CPP_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS(WCDB::TokenizerOneOrBinary, WCDB::TokenizerParameter_ChineseTraditionalToSimplified)
WCDB_CPP_ORM_IMPLEMENTATION_END

bool CPPFTS3Object::operator==(const CPPFTS3Object &other)
{
    return content.compare(other.content) == 0 && extension.compare(other.extension) == 0;
}

CPPFTS3SymbolObject::CPPFTS3SymbolObject() = default;

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPFTS3SymbolObject)

WCDB_CPP_SYNTHESIZE(content)
WCDB_CPP_VIRTUAL_TABLE_MODULE(WCDB::ModuleFTS3)
WCDB_CPP_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS(WCDB::TokenizerOneOrBinary, WCDB::TokenizerParameter_NeedSymbol)

WCDB_CPP_ORM_IMPLEMENTATION_END

bool CPPFTS3SymbolObject::operator==(const CPPFTS3SymbolObject &other)
{
    return content.compare(other.content) == 0;
}

//
// Created by qiuwenchen on 2022/9/7.
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

#include "CPPTestCaseObject.h"

CPPTestCaseObject::CPPTestCaseObject() = default;

CPPTestCaseObject::CPPTestCaseObject(int id, const char *text)
: identifier(id), content(text)
{
}

CPPTestCaseObject::CPPTestCaseObject(int id, const std::string &text)
: identifier(id), content(text)
{
}

CPPTestCaseObject::CPPTestCaseObject(int id) : identifier(id)
{
}

bool CPPTestCaseObject::operator==(const CPPTestCaseObject &other)
{
    return identifier == other.identifier && content.compare(other.content) == 0;
}

CPPTestCaseObject::~CPPTestCaseObject() = default;

CPPTestCaseObject CPPTestCaseObject::autoIncrementObject(const std::string &text)
{
    CPPTestCaseObject obj = CPPTestCaseObject(0, text);
    obj.isAutoIncrement = true;
    return obj;
}

CPPTestCaseObject CPPTestCaseObject::autoIncrementObject(const char *text)
{
    CPPTestCaseObject obj = CPPTestCaseObject(0, text);
    obj.isAutoIncrement = true;
    return obj;
}

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPTestCaseObject)
WCDB_CPP_SYNTHESIZE(identifier)
WCDB_CPP_SYNTHESIZE(content)
WCDB_CPP_PRIMARY_ASC_AUTO_INCREMENT(identifier)
WCDB_CPP_ORM_IMPLEMENTATION_END

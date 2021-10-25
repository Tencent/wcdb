//
// Created by 陈秋文 on 2021/10/17.
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

#include <WCDB/Assertion.hpp>
#include <WCDB/AuxiliaryFunctionModule.hpp>

namespace WCDB {

AbstractFTS5AuxiliaryFunctionObject::AbstractFTS5AuxiliaryFunctionObject(
int nVal, sqlite3_value** apVal, void* context)
{
    WCDB_UNUSED(nVal);
    WCDB_UNUSED(apVal);
    WCDB_UNUSED(context);
}

AbstractFTS5AuxiliaryFunctionObject::~AbstractFTS5AuxiliaryFunctionObject() = default;

FTS5AuxiliaryFunctionModule::FTS5AuxiliaryFunctionModule()
: m_func(nullptr), m_context(nullptr)
{
}

FTS5AuxiliaryFunctionModule::FTS5AuxiliaryFunctionModule(const AuxiliaryFunction& func,
                                                         void* context)
: m_func(func), m_context(context)
{
}

FTS5AuxiliaryFunctionModule::AuxiliaryFunction FTS5AuxiliaryFunctionModule::getFunc() const
{
    return m_func;
}

void* FTS5AuxiliaryFunctionModule::getContext() const
{
    return m_context;
}

} // namespace WCDB

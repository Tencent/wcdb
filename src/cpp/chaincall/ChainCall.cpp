//
// Created by 陈秋文 on 2022/8/29.
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

#include "ChainCall.hpp"
#include "Assertion.hpp"
#include "Handle.hpp"
#include "InnerHandle.hpp"

namespace WCDB {

BaseChainCall::BaseChainCall(Recyclable<InnerDatabase*> databaseHolder)
: m_handle(std::shared_ptr<Handle>(new Handle(databaseHolder)))
{
}

BaseChainCall::BaseChainCall(const BaseChainCall& other)
: m_handle(other.m_handle){

};

BaseChainCall::BaseChainCall(BaseChainCall&& other) : m_handle(other.m_handle)
{
}

BaseChainCall& BaseChainCall::operator=(const BaseChainCall& other)
{
    m_handle = other.m_handle;
    return *this;
}

BaseChainCall::~BaseChainCall() = default;

const Error& BaseChainCall::getError()
{
    return m_handle->getError();
}

void BaseChainCall::assertError(const UnsafeStringView& message)
{
    WCTRemedialAssert(false, message, return;);
}

} //namespace WCDB

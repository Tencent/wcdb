//
// Created by 陈秋文 on 2023/11/16.
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

#include "HandleOperator.hpp"

namespace WCDB {

HandleOperator::HandleOperator(InnerHandle *handle) : m_handle(handle)
{
}

HandleOperator::HandleOperator(HandleOperator &&other)
: m_handle(std::move(other.m_handle))
{
    other.m_handle = nullptr;
}

HandleOperator::~HandleOperator() = default;

InnerHandle *HandleOperator::getHandle() const
{
    return m_handle;
}

void HandleOperator::onDecorationChange()
{
}

} // namespace WCDB

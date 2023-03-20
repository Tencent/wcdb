//
// Created by sanhuazhang on 2018/12/28
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

#include "Recyclable.hpp"
#include "Assertion.hpp"
#include <atomic>

namespace WCDB {

Referenced::Referenced() : m_reference(new std::atomic<int>(0))
{
}

Referenced::Referenced(const std::nullptr_t&) : m_reference(nullptr)
{
}

Referenced::~Referenced() = default;

void Referenced::reset(const Referenced& other)
{
    other.retain();
    release();
    m_reference = other.m_reference;
}

void Referenced::reset(const std::nullptr_t&)
{
    release();
    m_reference = nullptr;
}

void Referenced::retain() const
{
    if (m_reference != nullptr) {
        ++(*m_reference);
    }
}

void Referenced::release()
{
    if (m_reference != nullptr) {
        WCTAssert((*m_reference) > 0);
        if (--(*m_reference) == 0) {
            onUnreferenced();
        }
    }
}
} // namespace WCDB

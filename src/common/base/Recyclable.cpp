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
#include "WCDBLog.hpp"
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
    WCDB2_LOGI("start reset, ref=%p, currentThisRef=%p", (void *) &other, (void *) this);
    other.retain();
    release();
    m_reference = other.m_reference;
    WCDB2_LOGI("end reset, ref=%p, m_reference=%d, currentThisRef=%p", (void *) &other,
               (m_reference != nullptr ? m_reference->load() : -1024), (void *) this);
}

void Referenced::reset(const std::nullptr_t&)
{
    if (m_reference != nullptr) {
        WCDB2_LOGI("start reset, m_referenceAddress=%p, currentThisRef=%p", m_reference.get(), (void *) this);
    } else {
        WCDB2_LOGI("start reset, m_reference is null, currentThisRef=%p", (void *) this);
    }
    release();
    m_reference = nullptr;
    WCDB2_LOGI("end reset, currentThisRef=%p", (void *) this);
}

void Referenced::retain() const
{
    if (m_reference != nullptr) {
        WCDB2_LOGI(
                "start retain,m_reference isNotNull, m_referenceAddress=%p, m_referenceCount=%d, currentThisRef=%p",
                m_reference.get(), m_reference->load(), (void *) this);
        ++(*m_reference);
        WCDB2_LOGI("end retain,m_reference isNotNull, m_referenceAddress=%p, m_referenceCount=%d, currentThisRef=%p",
                   m_reference.get(), m_reference->load(), (void *) this);
    } else {
        WCDB2_LOGI("start retain, but m_reference isNull, currentThisRef=%p", (void *) this);
    }
}

void Referenced::release()
{
    if (m_reference != nullptr) {
        WCDB2_LOGI("start release, m_reference isNotNull, m_referenceAddress=%p, m_referenceCount=%d,currentThisRef=%p",m_reference.get(), m_reference->load(), (void *) this);
        WCTAssert((*m_reference) > 0);
        if (--(*m_reference) == 0) {
            WCDB2_LOGI("start onUnreferenced, m_referenceAddress=%p, m_referenceCount=%d,currentThisRef=%p",m_reference.get(), m_reference->load(), (void *) this);
            onUnreferenced();
        } else {
            WCDB2_LOGI("oops, skip onUnreferenced, m_referenceAddress=%p, m_referenceCount=%d,currentThisRef=%p", m_reference.get(), m_reference->load(),(void *) this);
        }
    } else {
        WCDB2_LOGI("start release, m_reference is Null, currentThisRef=%p", (void *) this);
    }
}
} // namespace WCDB

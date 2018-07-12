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
#include <WCDB/ThreadLocal.hpp>

namespace WCDB {

#pragma mark - Threaded Info
UntypedThreadLocal::Info::Info(const Recyclable<pthread_key_t>& key,
                               const Constructor& constructor,
                               const Deconstructor& deconstructor)
: m_key(key), m_constructor(constructor), m_deconstructor(deconstructor), m_holder(nullptr)
{
    WCTInnerAssert(m_constructor != nullptr);
    WCTInnerAssert(m_deconstructor != nullptr);
}

UntypedThreadLocal::Info::~Info()
{
    if (m_holder) {
        m_deconstructor(m_holder);
    }
}

void* UntypedThreadLocal::Info::getOrCreate()
{
    if (m_holder == nullptr) {
        m_holder = m_constructor();
    }
    return m_holder;
}

#pragma mark - UntypedThreadLocal
UntypedThreadLocal::UntypedThreadLocal()
: m_identifier(nextIdentifier()), m_key(sharedKey())
{
}

void* UntypedThreadLocal::getOrCreate()
{
    Infos* infos = (Infos*) pthread_getspecific(m_key.unsafeGet());
    if (infos == nullptr) {
        infos = new Infos;
        pthread_setspecific(m_key.unsafeGet(), infos);
    }
    auto iter = infos->find(m_identifier);
    if (iter == infos->end()) {
        iter = infos
               ->emplace(
               m_identifier,
               Info(m_key,
                    std::bind(&UntypedThreadLocal::constructor, this),
                    std::bind(&UntypedThreadLocal::deconstructor, this, std::placeholders::_1)))
               .first;
    }
    return infos->at(m_identifier).getOrCreate();
}

#pragma mark - Shared
UntypedThreadLocal::Identifier UntypedThreadLocal::nextIdentifier()
{
    static std::atomic<Identifier>* s_identifier = new std::atomic<Identifier>(0);
    return ++(*s_identifier);
}

const Recyclable<pthread_key_t>& UntypedThreadLocal::sharedKey()
{
    static Recyclable<pthread_key_t>* s_key = new Recyclable<pthread_key_t>(
    constructKey(), UntypedThreadLocal::deconstructKey);
    return *s_key;
}

#pragma mark - Helper
void UntypedThreadLocal::threadDeconstructor(void* p)
{
    std::map<Identifier, Info>* infos = (std::map<Identifier, Info>*) p;
    delete infos;
}

pthread_key_t UntypedThreadLocal::constructKey()
{
    pthread_key_t key;
    pthread_key_create(&key, UntypedThreadLocal::threadDeconstructor);
    return key;
}

void UntypedThreadLocal::deconstructKey(pthread_key_t& key)
{
    pthread_key_delete(key);
}

} // namespace WCDB

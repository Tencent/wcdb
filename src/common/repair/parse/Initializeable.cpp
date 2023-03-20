//
// Created by sanhuazhang on 2018/06/10
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

#include "Initializeable.hpp"

namespace WCDB {

namespace Repair {

Initializeable::Initializeable() : m_state(State::None)
{
}

Initializeable::~Initializeable() = default;

bool Initializeable::isInitialized() const
{
    return m_state == State::Initialized;
}

bool Initializeable::isInitializing() const
{
    return m_state == State::Initializing;
}

bool Initializeable::isInitializeFalied() const
{
    return m_state == State::Failed;
}

bool Initializeable::initialize()
{
    if (m_state == State::None) {
        m_state = State::Initializing;
        m_state = doInitialize() ? State::Initialized : State::Failed;
    }
    return m_state == State::Initialized;
}

} //namespace Repair

} //namespace WCDB

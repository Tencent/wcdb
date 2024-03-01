//
// Created by sanhuazhang on 2019/05/23
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

#pragma once

#include "CrossPlatform.h"
#include "Macro.h"
#include "SharedThreadedErrorProne.hpp"
#include "StringView.hpp"
#include <cstddef>
#include <thread>

namespace WCDB {

class Thread final : public SharedThreadedErrorProne {
#pragma mark - Initialize
public:
    Thread();
    ~Thread() override;
    Thread& operator=(const std::nullptr_t&);

private:
    Thread(std::thread::id id
#if defined(__APPLE__)
           ,
           pthread_t pthreadId
#endif
    );
    friend class Conditional;
    std::thread::id m_id;
#if defined(__APPLE__)
    pthread_t m_pthreadId;
#endif
    static std::thread::id m_uiThreadId;

#pragma mark - Which
public:
    static Thread current();
    static bool isMain();
    static void setUIThreadId(std::thread::id threadId);
    static uint64_t getCurrentThreadId();
    bool isCurrentThread() const;

    bool equal(const Thread& other) const;
    bool operator==(const Thread& other) const;

private:
#pragma mark - Name
public:
    static void setName(const UnsafeStringView& name);

private:
    static constexpr int maxLengthOfAllowedThreadName();
};

} // namespace WCDB

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

#include "Thread.hpp"
#include "Assertion.hpp"
#include "CrossPlatform.h"
#include "Notifier.hpp"
#include "WCDBError.hpp"
#include <cstring>
#if _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#ifdef __linux__
#include <sys/syscall.h>
#endif
#endif

namespace WCDB {

#pragma mark - Initialize
Thread::Thread(std::thread::id threadId
#if defined(__APPLE__)
               ,
               pthread_t pthreadId
#endif
               )
: m_id(threadId)
#if defined(__APPLE__)
, m_pthreadId(pthreadId)
#endif
{
}

Thread& Thread::operator=(const std::nullptr_t&)
{
    m_id = std::thread::id();
#if defined(__APPLE__)
    m_pthreadId = nullptr;
#endif
    return *this;
}

Thread::Thread()
: m_id()
#if defined(__APPLE__)
, m_pthreadId(nullptr)
#endif
{
}

Thread::~Thread() = default;

#pragma mark - Which
Thread Thread::current()
{
#if defined(__APPLE__)
    return Thread(std::this_thread::get_id(), pthread_self());
#else
    return Thread(std::this_thread::get_id());
#endif
}

bool Thread::isMain()
{
#ifndef _WIN32
    return pthread_main_np() != 0;
#else
    return std::this_thread::get_id() == m_uiThreadId;
#endif
}

bool Thread::isCurrentThread() const
{
    return m_id == std::this_thread::get_id();
}

bool Thread::equal(const Thread& other) const
{
    return m_id == other.m_id;
}

bool Thread::operator==(const Thread& other) const
{
    return equal(other);
}

std::thread::id Thread::m_uiThreadId;

void Thread::setUIThreadId(std::thread::id threadId)
{
    m_uiThreadId = threadId;
}

uint64_t Thread::getCurrentThreadId()
{
#ifdef __APPLE__
    uint64_t tid = 0;
    pthread_threadid_np(nullptr, &tid);
    return tid;
#elif _WIN32
    return (uint64_t) GetCurrentThreadId();
#elif __linux__
    return (uint64_t) syscall(SYS_gettid);
#else
    return (uint64_t)::gettid();
#endif
}

#pragma mark - Name
constexpr int Thread::maxLengthOfAllowedThreadName()
{
    return 256;
}

void Thread::setName(const UnsafeStringView& name)
{
#ifndef _WIN32
    WCTAssert(name.length() <= 16) // Thread name is limited to 16 bytes in linus;
    char buffer[maxLengthOfAllowedThreadName()];
    memset(buffer, 0, maxLengthOfAllowedThreadName());
    memcpy(buffer, name.data(), name.length());
    int ret = pthread_setname_np(buffer);
    if (ret != 0) {
        Error error;
        error.level = Error::Level::Warning;
        error.setSystemCode(ret, Error::Code::Error);
        error.infos.insert_or_assign("Operation", "setThreadName");
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
    }
#endif
}

} // namespace WCDB

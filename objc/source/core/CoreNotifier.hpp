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

#ifndef CoreNotifier_hpp
#define CoreNotifier_hpp

#include <WCDB/Error.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/Notifier.hpp>

namespace WCDB {

class CorruptionQueue;

class CoreNotifier {
public:
    CoreNotifier();
    CoreNotifier(const CoreNotifier&) = delete;
    CoreNotifier& operator=(const CoreNotifier&) = delete;

    using Callback = Notifier::Callback;
    void setNotification(const Callback& callback);

    void setCorruptionQueue(CorruptionQueue* queue);

    static void logger(const Error& error);
    static void globalLogger(void* userInfo, int code, const char* message);

protected:
    void notify(const Error& error);

    SharedLock m_lock;
    Callback m_callback;
    CorruptionQueue* m_corruptionQueue;
};

} // namespace WCDB

#endif /* CoreNotifier_hpp */

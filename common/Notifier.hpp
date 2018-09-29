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

#ifndef Reporter_hpp
#define Reporter_hpp

#include <WCDB/Error.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/OrderedUniqueList.hpp>

namespace WCDB {

class Notifier {
public:
    static Notifier *shared();
    Notifier(const Notifier &) = delete;
    Notifier &operator=(const Notifier &) = delete;

    void notify(const Error &error) const;

    typedef std::function<void(const Error &)> Callback;
    void setNotification(const std::string &key, const Callback &callback);

    typedef std::function<void(Error &)> PreprocessCallback;
    void setNotificationForPreprocessing(const std::string &key,
                                         const PreprocessCallback &callback);

protected:
    Notifier();

    mutable SharedLock m_lock;

    std::map<std::string, Callback> m_notifications;
    std::map<std::string, PreprocessCallback> m_preprocessNotifications;
};

} //namespace WCDB

#endif /* Reporter_hpp */

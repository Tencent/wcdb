//
// Created by sanhuazhang on 2018/06/07
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

#include "Lock.hpp"
#include "UniqueList.hpp"
#include "WCDBError.hpp"

namespace WCDB {

class Notifier final {
public:
    static Notifier &shared();

    void notify(Error &error) const;

    typedef std::function<void(const Error &)> Callback;
    void setNotification(int order, const UnsafeStringView &key, const Callback &callback);
    void unsetNotification(const UnsafeStringView &key);

    typedef std::function<void(Error &error)> PreprocessCallback;
    void setNotificationForPreprocessing(const UnsafeStringView &key,
                                         const PreprocessCallback &callback);

protected:
    Notifier();
    Notifier(const Notifier &) = delete;
    Notifier &operator=(const Notifier &) = delete;

    mutable SharedLock m_lock;

    UniqueList<StringView, Callback> m_notifications;
    StringViewMap<PreprocessCallback> m_preprocessNotifications;
};

} //namespace WCDB

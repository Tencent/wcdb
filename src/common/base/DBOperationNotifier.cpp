//
// Created by qiuwenchen on 2022/9/28.
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

#include "DBOperationNotifier.hpp"
#include "Assertion.hpp"
#include "InnerDatabase.hpp"

namespace WCDB {

DBOperationNotifier::DBOperationNotifier() = default;

DBOperationNotifier &DBOperationNotifier::shared()
{
    static DBOperationNotifier *s_notifier = new DBOperationNotifier;
    return *s_notifier;
}

void DBOperationNotifier::setNotification(const Callback &callback)
{
    LockGuard lockGuard(m_lock);
    if (callback != nullptr) {
        WCTAssert(m_notification == nullptr);
    }
    m_notification = callback;
}

void DBOperationNotifier::notifyOperation(InnerDatabase *database,
                                          Operation operation,
                                          StringViewMap<Value> &info) const
{
    SharedLockGuard lockGuard(m_lock);
    if (m_notification) {
        m_notification(database, operation, info);
    }
}

} // namespace WCDB

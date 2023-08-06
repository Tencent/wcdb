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

#pragma once

#include "Lock.hpp"
#include "Value.hpp"

namespace WCDB {

class InnerDatabase;

class DBOperationNotifier final {
public:
    static DBOperationNotifier &shared();

    enum Operation : int {
        Create = 0,
        SetTag = 1,
        OpenHandle = 2,
    };

    void notifyOperation(InnerDatabase *database,
                         Operation operation,
                         StringViewMap<Value> &info) const;

    typedef std::function<void(InnerDatabase *database, Operation operation, StringViewMap<Value> &info)> Callback;

    void setNotification(const Callback &callback);

protected:
    DBOperationNotifier();
    DBOperationNotifier(const DBOperationNotifier &) = delete;
    DBOperationNotifier &operator=(const DBOperationNotifier &) = delete;

    mutable SharedLock m_lock;
    Callback m_notification;
};

} //namespace WCDB

//
// Created by qiuwenchen on 2022/9/13.
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

namespace WCDB {

class InnerHandle;

class TransactionEvent {
public:
    virtual void markHandleAsTransactioned(InnerHandle* handle) = 0;
    virtual void markHandleAsUntransactioned() = 0;
    virtual ~TransactionEvent() = 0;
};

class TransactionGuard final {
public:
    TransactionGuard(TransactionEvent* event, InnerHandle* handle);
    ~TransactionGuard();

private:
    TransactionEvent* m_event;
    InnerHandle* m_handle;
    bool m_isInTransactionBefore;
};

} //namespace WCDB

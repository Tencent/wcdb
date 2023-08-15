//
// Created by 陈秋文 on 2023/8/13.
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

#include "CoreConst.h"
#include "Lock.hpp"
#include "Thread.hpp"

namespace WCDB {

class HandleCounter {
public:
    HandleCounter();
    ~HandleCounter();
    HandleCounter(const HandleCounter &) = delete;
    HandleCounter &operator=(const HandleCounter &) = delete;

    bool tryIncreaseHandleCount(HandleType type, bool writeHint);
    void decreaseHandleCount(bool writeHint);

private:
    mutable std::mutex m_lock;
    Conditional m_conditionalNormals;
    Conditional m_conditionalWriters;
    std::queue<Thread> m_pendingNormals;
    std::queue<Thread> m_pendingWriters;
    int m_writerCount = 0;
    int m_totalCount = 0;
};

} // namespace WCDB

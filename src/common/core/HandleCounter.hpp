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

/*
 * When obtaining a database handle, you can indicate whether
 * this handle will be used to write new data to the database.
 *
 * The handle counter is used to limit the number of handles in two ways:
 * 1. Only 4 handles can exist for writing at the same time.
 *    Because it is difficult to accurately distinguish whether the handle is used for writing,
 *    and there is still a little time between getting the handle and writing data,
 *    let the four handles fully compete.
 * 2. Only up to 32 handles can exist at the same time.
 *    Too many handles not only take up memory, but generally imply inappropriate usage.
 *
 * When the number limit is exceeded, the handle counter will let the thread
 * that acquires the handle wait in place until other handles are recycled.
 */

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

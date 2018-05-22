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

#ifndef ThreadedHandleErrorProne_hpp
#define ThreadedHandleErrorProne_hpp

#include <WCDB/Error.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <unordered_map>

namespace WCDB {

class HandlePool;

class ThreadedHandleErrorProne {
protected:
    void setThreadedError(const Error &error) const;
    void setThreadedError(Error &&error) const;
    const Error &getThreadedError() const;
    virtual const HandlePool *getErrorAssociatedHandlePool() const = 0;
    void error(const std::string &message) const;

private:
    using ThreadedErrors = std::unordered_map<const HandlePool *, Error>;
    static ThreadLocal<ThreadedErrors> *threadedErrors();
};

} //namespace WCDB

#endif /* ThreadedHandleErrorProne_hpp */

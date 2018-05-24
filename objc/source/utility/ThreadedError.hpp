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

#ifndef ThreadedError_hpp
#define ThreadedError_hpp

#include <WCDB/Error.hpp>
#include <WCDB/ThreadLocal.hpp>

namespace WCDB {

class ThreadedErrors {
public:
    static ThreadedErrors *shared();
    const Error &getThreadedError();

protected:
    friend class ThreadedErrorProne;
    void setThreadedError(const Error &error);
    void setThreadedError(Error &&error);

private:
    ThreadLocal<Error> m_errors;
};

class ThreadedErrorProne {
public:
    void setThreadedError(const Error &error);
    void setThreadedError(Error &&error);
    const Error &getThreadedError();

protected:
    virtual ThreadedErrors *getThreadedErrors() = 0;
};

class SharedThreadedErrorProne : protected ThreadedErrorProne {
public:
    using ThreadedErrorProne::getThreadedError;

protected:
    ThreadedErrors *getThreadedErrors() override;
};

} //namespace WCDB

#endif /* ThreadedError_hpp */

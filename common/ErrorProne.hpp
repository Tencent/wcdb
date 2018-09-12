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

#ifndef ErrorProne_hpp
#define ErrorProne_hpp

#include <WCDB/Error.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

class ErrorProne {
public:
    const Error &getError() const;

protected:
    void assignWithSharedThreadedError();
    void setError(const Error &error);
    void setError(Error &&error);

    Error m_error;
};

class ThreadedErrorProne {
public:
    const Error &getThreadedError();

protected:
    void setThreadedError(const Error &error);
    void setThreadedError(Error &&error);
    void assignWithSharedThreadedError();

private:
    ThreadedErrors m_threadedError;
};

} //namespace WCDB

#endif /* ErrorProne_hpp */

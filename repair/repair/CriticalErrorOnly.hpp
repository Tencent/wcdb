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

#ifndef CriticalErrorOnly_hpp
#define CriticalErrorOnly_hpp

#include <WCDB/Error.hpp>

namespace WCDB {

namespace Repair {

class CriticalErrorOnly {
public:
    CriticalErrorOnly();
    enum CriticalLevel {
        MostFatal = std::numeric_limits<int>::max(),
        Fatal = 1,
        NotFatal = 0,
        None = std::numeric_limits<int>::min(),
    };
    const Error &getCriticalError() const;
    int getCriticalLevel() const;

protected:
    static int criticalLevel(const Error &error);
    int tryUpgradeError(const Error &newError);
    int tryUpgradeError(Error &&newError);
    int tryUpgradeErrorWithSharedThreadedError();
    void setCriticalError(const Error &error);
    void setCriticalError(Error &&error);
    void setCriticalErrorWIthSharedThreadedError();

    virtual void onErrorCritical();

private:
    Error m_criticalError;
    int m_criticalLevel;
};

} //namespace Repair

} //namespace WCDB

#endif /* CriticalErrorOnly_hpp */

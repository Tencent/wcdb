//
// Created by sanhuazhang on 2018/09/26
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

#include "ErrorProne.hpp"
#include <limits>

namespace WCDB {

namespace Repair {

class UpgradeableErrorProne : private ErrorProne {
public:
    UpgradeableErrorProne();
    virtual ~UpgradeableErrorProne() override = 0;
    enum Severity {
        MostCritical = std::numeric_limits<int>::max(),
        Critical = 1,
        Normal = 0,
        None = std::numeric_limits<int>::min(),
    };

    using ErrorProne::getError;
    int getErrorSeverity() const;
    bool isErrorCritial() const;

protected:
    int errorSeverity(const Error &error);

    int tryUpgradeError(const Error &newError);
    int tryUpgradeError(Error &&newError);
    int tryUpgradeErrorWithSharedThreadedError();

    void setCriticalError(const Error &error);
    void setCriticalError(Error &&error);
    void setCriticalErrorWithSharedThreadedError();

    virtual void onErrorCritical();

private:
    void setSeverity(int severity);
    int m_severity;
};

} //namespace Repair

} //namespace WCDB

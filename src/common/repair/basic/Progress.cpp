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

#include "Progress.hpp"
#include "Assertion.hpp"

namespace WCDB {

namespace Repair {

Progress::Progress() : m_progress(0), m_onProgressUpdated(nullptr)
{
}

Progress::~Progress() = default;

bool Progress::increaseProgress(double increment)
{
    double progress = m_progress + increment;
    return updateProgress(progress > 0.9999 ? 0.9999 : progress);
}

bool Progress::finishProgress()
{
    return updateProgress(1);
}

bool Progress::updateProgress(double progress)
{
    double increment = progress - m_progress;
    WCTAssert(increment >= 0);
    if (increment > 0) {
        m_progress = progress;
        if (m_onProgressUpdated != nullptr) {
            return m_onProgressUpdated(m_progress, increment);
        }
    }
    return true;
}

void Progress::setProgressCallback(const ProgressUpdateCallback &onProgressUpdated)
{
    m_onProgressUpdated = onProgressUpdated;
}

} //namespace Repair

} //namespace WCDB

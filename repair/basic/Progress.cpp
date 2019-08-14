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

#include <WCDB/Assertion.hpp>
#include <WCDB/Progress.hpp>

namespace WCDB {

namespace Repair {

Progress::Progress() : m_progress(0), m_onProgressUpdated(nullptr)
{
}

Progress::~Progress() = default;

void Progress::increaseProgress(double increment)
{
    double progress = m_progress + increment;
    updateProgress(progress > 0.9999 ? 0.9999 : progress);
}

void Progress::finishProgress()
{
    updateProgress(1);
}

void Progress::updateProgress(double progress)
{
    double increment = progress - m_progress;
    WCTAssert(increment >= 0);
    if (increment > 0) {
        m_progress = progress;
        if (m_onProgressUpdated != nullptr) {
            m_onProgressUpdated(m_progress, increment);
        }
    }
}

void Progress::setProgressCallback(const ProgressUpdateCallback &onProgressUpdated)
{
    m_onProgressUpdated = onProgressUpdated;
}

} //namespace Repair

} //namespace WCDB

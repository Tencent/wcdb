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
#include <algorithm>

namespace WCDB {

Progress::Progress() : m_progress(0), m_onProgressUpdated(nullptr)
{
}

Progress::~Progress() = default;

void Progress::clearProgress()
{
    m_progress = 0;
}

bool Progress::increaseProgress(double increment)
{
    return updateProgress(m_progress + increment);
}

bool Progress::updateProgress(double progress, bool force)
{
    if (!force) {
        progress = std::min(progress, 0.999);
    }
    double increment = progress - m_progress;
    WCTAssert(increment >= 0);
    if (increment > 0.001) {
        m_progress = progress;
        if (m_onProgressUpdated != nullptr) {
            return m_onProgressUpdated(m_progress, increment);
        }
    }
    return true;
}

bool Progress::finishProgress()
{
    return updateProgress(1, true);
}

void Progress::setProgressCallback(const ProgressUpdateCallback &onProgressUpdated)
{
    m_onProgressUpdated = onProgressUpdated;
}

} //namespace WCDB

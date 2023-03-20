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

#include "Scoreable.hpp"
#include "Assertion.hpp"
#include "Console.hpp"
#include <cmath>

namespace WCDB {

namespace Repair {

Scoreable::~Scoreable() = default;

const Fraction &Scoreable::getScore() const
{
    return m_score;
}

void Scoreable::increaseScore(const Fraction &increment)
{
    m_score += increment;
    WCTAssert(m_score.value() <= 1.0);
    if (m_score.value() > 1.0) {
        m_score = 1;
    }
}

SegmentedScoreable::~SegmentedScoreable() = default;

const Fraction &SegmentedScoreable::getScore() const
{
    WCTAssert(m_segmentedScore.value() == 0);
    return Scoreable::getScore();
}

void SegmentedScoreable::markSegmentedScoreCounted()
{
    Scoreable::increaseScore(m_segmentedScore);
    m_segmentedScore = 0;
}

void SegmentedScoreable::markSegmentedScoreDropped()
{
    m_segmentedScore = 0;
}

void SegmentedScoreable::increaseScore(const Fraction &increment)
{
    m_segmentedScore += increment;
    WCTAssert((Scoreable::getScore() + m_segmentedScore).value() <= 1.0);
}

} //namespace Repair

} //namespace WCDB

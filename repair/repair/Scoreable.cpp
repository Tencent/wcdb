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
#include <WCDB/Scoreable.hpp>
#include <cmath>

namespace WCDB {

namespace Repair {

Scoreable::Scoreable() : m_score(0)
{
}

double Scoreable::getScore() const
{
    return round(m_score * 10000) / 10000;
}

void Scoreable::increaseScore(double increment)
{
    WCTInnerAssert(increment >= 0);
    if (increment > 0) {
        updateScore(m_score + increment);
    }
}

void Scoreable::updateScore(double score)
{
    WCTInnerAssert(score >= m_score);
    WCTInnerAssert(score <= 1.0);
    if (score > m_score) {
        m_score = std::min(score, 1.0);
    }
}

FractionalScoreable::FractionalScoreable() : Scoreable(), m_fractionalScore(0)
{
}

double FractionalScoreable::getScore() const
{
    WCTInnerAssert(m_fractionalScore == 0);
    return Scoreable::getScore();
}

void FractionalScoreable::markFractionalScoreCounted()
{
    if (m_fractionalScore > 0) {
        Scoreable::increaseScore(m_fractionalScore);
        m_fractionalScore = 0;
    }
}

void FractionalScoreable::increaseScore(double increment)
{
    WCTInnerAssert(increment >= 0);
    if (increment > 0) {
        updateScore(m_fractionalScore + increment);
    }
}

void FractionalScoreable::updateScore(double score)
{
    WCTInnerAssert(score >= m_fractionalScore);
    WCTInnerAssert(score + Scoreable::getScore() <= 1.0);
    if (score > m_fractionalScore) {
        m_fractionalScore = std::min(score, 1.0 - Scoreable::getScore());
    }
}

} //namespace Repair

} //namespace WCDB

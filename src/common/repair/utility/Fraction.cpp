//
// Created by sanhuazhang on 2018/06/21
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

#include "Fraction.hpp"
#include "Assertion.hpp"

namespace WCDB {

namespace Repair {

Fraction::Fraction() : m_numerator(0), m_denominator(1)
{
}

Fraction::Fraction(int64_t value) : m_numerator(value), m_denominator(1)
{
}

Fraction::Fraction(int64_t numerator, int64_t denominator)
: m_numerator(numerator), m_denominator(denominator)
{
    WCTRemedialAssert(denominator != 0, "Denominator can't be 0.", m_numerator = 0;
                      m_denominator = 1;);
}

int64_t Fraction::euclidean(int64_t a, int64_t b) const
{
    while (b != 0) {
        std::tie(a, b) = std::make_pair(b, a % b);
    }
    if (a == 0) {
        a = 1;
    }
    return a;
}

void Fraction::reduce()
{
    int64_t gcd = euclidean(m_numerator, m_denominator);
    if (gcd > 1) {
        m_numerator /= gcd;
        m_denominator /= gcd;
    }
}

int64_t Fraction::limitNumber(int64_t a, int64_t b) const
{
    int64_t limit = 1;
    a = std::abs(a);
    b = std::abs(b);
    int64_t max_b = std::numeric_limits<int64_t>::max() / a;
    if (max_b < b) {
        limit = b / max_b + (b % max_b > 0 ? 1 : 0);
    }
    return limit;
}

Fraction Fraction::operator+(const Fraction &operand) const
{
    int64_t numerator, denominator;
    if (m_denominator == operand.m_denominator) {
        numerator = m_numerator + operand.m_numerator;
        denominator = m_denominator;
    } else {
        int64_t gcd = euclidean(m_denominator, operand.m_denominator);

        denominator = m_denominator / gcd;
        int64_t limit = limitNumber(denominator, operand.m_denominator);
        gcd *= limit;
        denominator /= limit;

        numerator = (operand.m_denominator / gcd * m_numerator)
                    + (m_denominator / gcd * operand.m_numerator);
        denominator = denominator * operand.m_denominator;
    }
    Fraction result(numerator, denominator);
    result.reduce();
    return result;
}

Fraction Fraction::operator*(const Fraction &operand) const
{
    int64_t gcd1 = euclidean(m_numerator, operand.m_denominator);
    int64_t gcd2 = euclidean(operand.m_numerator, m_denominator);

    int64_t denominator1 = m_denominator / gcd2;
    int64_t denominator2 = operand.m_denominator / gcd1;
    int64_t limit = limitNumber(denominator1, denominator2);
    gcd2 *= limit;
    denominator1 /= limit;

    Fraction result(m_numerator / gcd1 * (operand.m_numerator / gcd2),
                    denominator1 * denominator2);
    return result;
}

Fraction &Fraction::operator+=(const Fraction &operand)
{
    if (m_denominator == operand.m_denominator) {
        m_numerator += operand.m_numerator;
    } else {
        int64_t gcd = euclidean(m_denominator, operand.m_denominator);

        int64_t denominator = operand.m_denominator / gcd;
        int64_t limit = limitNumber(m_denominator, denominator);
        gcd *= limit;
        denominator /= limit;

        m_numerator = (operand.m_denominator / gcd * m_numerator)
                      + (m_denominator / gcd * operand.m_numerator);
        m_denominator *= denominator;
    }
    reduce();
    return *this;
}

Fraction Fraction::operator/(const Fraction &operand) const
{
    int64_t gcd1 = euclidean(m_numerator, operand.m_numerator);
    int64_t gcd2 = euclidean(m_denominator, operand.m_denominator);
    Fraction result(m_numerator / gcd1 * (operand.m_denominator / gcd2),
                    m_denominator / gcd2 * (operand.m_numerator / gcd1));
    return result;
}

bool Fraction::operator<(const Fraction &operand) const
{
    int64_t gcd1 = euclidean(m_numerator, operand.m_numerator);
    int64_t gcd2 = euclidean(m_denominator, operand.m_denominator);
    return (m_numerator / gcd1 * (operand.m_denominator / gcd2))
           < (m_denominator / gcd2 * (operand.m_numerator / gcd1));
}

double Fraction::value() const
{
    return (double) m_numerator / m_denominator;
}

int64_t Fraction::numberator() const
{
    return m_numerator;
}

int64_t Fraction::denominator() const
{
    return m_denominator;
}

} //namespace Repair

} //namespace WCDB

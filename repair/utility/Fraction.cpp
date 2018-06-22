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
#include <WCDB/Fraction.hpp>

namespace WCDB {

namespace Repair {

Fraction::Fraction() : m_numerator(0), m_denominator(1)
{
}

Fraction::Fraction(int value) : m_numerator(value), m_denominator(1)
{
}

Fraction::Fraction(int numerator, int denominator)
    : m_numerator(numerator), m_denominator(denominator)
{
    WCTAssert(denominator != 0, "Denominator can't be 0.");
}

int Fraction::euclidean(int a, int b)
{
    return b == 0 ? a : euclidean(b, a % b);
}

void Fraction::reduce()
{
    int gcd = euclidean(m_numerator, m_denominator);
    if (gcd > 1) {
        m_numerator /= gcd;
        m_denominator /= gcd;
    }
}

Fraction Fraction::operator+(const Fraction &operand) const
{
    int numerator, denumberator;
    if (m_denominator == operand.m_denominator) {
        numerator = m_numerator + operand.m_numerator;
        denumberator = m_denominator;
    } else {
        numerator = (m_numerator * operand.m_denominator) +
                    (operand.m_numerator * m_denominator);
        denumberator = m_denominator * operand.m_denominator;
    }
    Fraction result(numerator, denumberator);
    result.reduce();
    return result;
}

Fraction Fraction::operator*(const Fraction &operand) const
{
    Fraction result(m_numerator * operand.m_numerator,
                    m_denominator * operand.m_denominator);
    result.reduce();
    return result;
}

Fraction &Fraction::operator+=(const Fraction &operand)
{
    if (m_denominator == operand.m_denominator) {
        m_numerator += operand.m_numerator;
    } else {
        m_numerator = (m_numerator * operand.m_denominator) +
                      (operand.m_numerator * m_denominator);
        m_denominator *= operand.m_denominator;
    }
    reduce();
    return *this;
}

Fraction Fraction::operator/(const Fraction &operand) const
{
    Fraction result(m_numerator * operand.m_denominator,
                    m_denominator * operand.m_numerator);
    result.reduce();
    return result;
}

bool Fraction::operator<(const Fraction &operand) const
{
    return (m_numerator * operand.m_denominator) <
           (operand.m_numerator * m_denominator);
}

double Fraction::value() const
{
    return (double) m_numerator / m_denominator;
}

} //namespace Repair

} //namespace WCDB
